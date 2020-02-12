function [dataOut]=FAMOSimport(filename)
% Usage: data=FAMOSimport(filename);
% 
% FAMOSimport() opens (MARC generated) FAMOS files and imports all signals.
% 
% ************************************************************************* 
% 
%


% Preset output to empty;
dataOut=[];


%% Check for valid input file
if exist('filename','var')~=1 ...
   || isempty(filename)
    [filename, pathname] = uigetfile( ...
        {'*.dat','FAMOS measurement files'; ...
         '*.*','All files'},'Select FAMOS measurement file ...');
     if isequal(filename,0)
         disp('FAMOS-measurement file import cancelled.');
         return;
     end
     filename=fullfile(pathname, filename);
     clear pathname;
end
if exist(filename,'file')~=2
    disp('Given file could not be found. Aborting import.');
    return;
end

%% Load input file 
fid=fopen(filename,'r','l');
data=fread(fid,inf,'uint8=>char','l')';
fclose(fid);
clear fid 


%% Parse header information
dataOut.FileName=filename;
dataOut.TYPE='AFT MARC (FAMOS)';
header=strfind(data(1:200),[char(13) char(10) '|NO,']);
subIdx=strfind(data(header(1):header(1)+50),',');
dataOut.Device=strtrim(data(header(1)+subIdx(5):header(1)+subIdx(6)-2));

%% Parse measurement entries information
units=strfind(data,[char(13) char(10) '|CR,'])';
dataOut.Unit=cell(size(units));
dataOut.Factor=zeros(size(dataOut.Unit),'double');
dataOut.Offset=zeros(size(dataOut.Unit),'double');
for i=1:length(units)
    subIdx=sort([strfind(data(units(i):units(i)+255),',') ...
                strfind(data(units(i):units(i)+255),';')]);
	dataOut.Factor(i)=str2double( ...
        data(units(i)+subIdx(4):units(i)+subIdx(5)-2));
	dataOut.Offset(i)=str2double( ...
        data(units(i)+subIdx(5):units(i)+subIdx(6)-2));
    dataOut.Unit{i}=data(units(i)+subIdx(8):units(i)+subIdx(9)-2);
    
end
clear units subIdx;

%Extract measurement variables names and corresponding time column
varName=strfind(data,[char(13) char(10) '|CN,'])';
dataOut.TimeIndex=zeros(size(varName),'uint32');
dataOut.Label=cellstr(char(zeros(size(dataOut.TimeIndex))));
dataOut.Data=zeros(size(dataOut.TimeIndex),'double');
for i=1:length(varName)
    subIdx=strfind(data(varName(i):varName(i)+255),',');
    dataOut.Label{i}=data(varName(i)+subIdx(7):varName(i)+subIdx(8)-2);
end
for i=1:length(varName)
    subIdx=sort([strfind(data(varName(i):varName(i)+255),',') ...
                   strfind(data(varName(i):varName(i)+255),';')]);
    TimeVarName=data(varName(i)+subIdx(9):varName(i)+subIdx(10)-2);
    if i==1 || ~strcmp(dataOut.TimeIndex(i-1),TimeVarName)
        idx=strmatch(TimeVarName,dataOut.Label,'exact');
        if ~isempty(idx)
            dataOut.TimeIndex(i)=idx(1);
        else
            warning('FAMOSconnect:invalidTimeLabel', ...
                ['Signal ''%s'' (%d) refers to non-existing ' ...
                'time signal label ''%s''.'], ...
                dataOut.Label{i},i,TimeVarName);    
        end
    else
        dataOut.TimeIndex(i)=dataOut.TimeIndex(i-1);
    end
end
clear varName TimeVarName subIdx;

%Extract measurement value data type and bitlength
dataType=strfind(data,[char(13) char(10) '|CP,1,'])';
dataOut.DataType=cell(size(dataType));
dataOut.DataBits=zeros(size(dataOut.DataType),'uint8');
for i=1:length(dataType)
    subIdx=strfind(data(dataType(i):dataType(i)+50),',');
    switch (data(dataType(i)+subIdx(5):dataType(i)+subIdx(6)-2))
        case '1'                        % uint8
            dataOut.DataType{i}='uint8';
        case '2'                        % int8
            dataOut.DataType{i}='int8';
        case {'3','9','11'}             % uint16
            dataOut.DataType{i}='uint16';
        case '4'                        % int16
            dataOut.DataType{i}='int16';
        case '5'                        % uint32
            dataOut.DataType{i}='uint32';
        case '6'                        % int32
            dataOut.DataType{i}='int32';
        case '7'                        % float
            dataOut.DataType{i}='single';
        case {'8','10','13'}            % double
            dataOut.DataType{i}='double';
        otherwise
            dataOut.DataType{i}='UNKNOWN';
    end 
    dataOut.DataBits(i)=str2double( ...
        data(dataType(i)+subIdx(6):dataType(i)+subIdx(7)-2));
end
clear dataType subIdx;

%Extract measurement value data block and item number
dataInfo=strfind(data,[char(13) char(10) '|Cb,1,'])';
dataOut.DataBlock=zeros(size(dataInfo),'uint16');
dataOut.DataItem=zeros(size(dataOut.DataBlock),'uint16');
for i=1:length(dataInfo)
    subIdx=strfind(data(dataInfo(i):dataInfo(i)+50),',');
    dataOut.DataItem(i)=str2double( ...
        data(dataInfo(i)+subIdx(5):dataInfo(i)+subIdx(6)-2));
    dataOut.DataBlock(i)=str2double( ...
        data(dataInfo(i)+subIdx(6):dataInfo(i)+subIdx(7)-2));
end
clear dataInfo subIdx;


%Extract measurement value binary data length and offset
dataBlock=strfind(data,[char(13) char(10) '|CS,'])';
dataOut.DataBlocks=length(dataBlock);
dataOut.DataBlocksLength=zeros(size(dataBlock),'uint32');
dataOut.DataBlocksItemLength=zeros(size(dataBlock),'uint32');
dataOut.DataBlocksOffset=zeros(size(dataBlock),'uint32');
for i=1:length(dataBlock)
    subIdx=strfind(data(dataBlock(i):dataBlock(i)+50),',');
    
    dataOut.DataBlocksOffset(i)=dataBlock(i)+subIdx(4)-1;
    dataOut.DataBlocksLength(i)=str2double( ...
        data(dataBlock(i)+subIdx(2):dataBlock(i)+subIdx(3)-2)) ...
        -(subIdx(4)-subIdx(3));                                 %Fix offset
    dataOut.DataBlocksItemLength(i)=dataOut.DataBlocksLength(i) ...
        /(sum(dataOut.DataBits(dataOut.DataBlock==i)/8));
end
clear dataBlock subIdx;


%% Sort entries - note: DataItem-value continues over blocks.
[~, dataOrder]=sort(dataOut.DataItem);
dataOutField=fieldnames(dataOut);
for i=1:length(dataOutField)
    if length(dataOut.(dataOutField{i}))==length(dataOut.DataItem) ...
       && ~strcmp(dataOutField{i},'DataBlocksLength')
        dataOut.(dataOutField{i})=dataOut.(dataOutField{i})(dataOrder);
    end
end
clear dataOrder dataOutField;


%% Extract measurement data, format: shots-aligned (not variables aligned)
data=cast(data,'uint8');
dataOut.Data=cell(length(dataOut.DataItem),1);
dataBlockId=1;
dataOffset=dataOut.DataBlocksOffset(dataBlockId);
dataVarIdx1=uint32(1: ...
                   dataOut.DataBlocksLength(dataBlockId) ...
                   /dataOut.DataBlocksItemLength(dataBlockId): ...
                   dataOut.DataBlocksLength(dataBlockId));
dataVarIdx2=reshape([dataVarIdx1;    dataVarIdx1+1],1,[]);
dataVarIdx4=reshape([dataVarIdx1;    dataVarIdx1+1; ...
                     dataVarIdx1+2;  dataVarIdx1+3],1,[]);
for i=1:length(dataOut.Label)
    if dataOut.DataBlock(i)>dataBlockId
        dataBlockId=dataOut.DataBlock(i);
        dataOffset=dataOut.DataBlocksOffset(dataBlockId);
        dataVarIdx1=uint32(1: ...
                           dataOut.DataBlocksLength(dataBlockId) ...
                           /dataOut.DataBlocksItemLength(dataBlockId): ...
                           dataOut.DataBlocksLength(dataBlockId));
        dataVarIdx2=reshape([dataVarIdx1;    dataVarIdx1+1],1,[]);
        dataVarIdx4=reshape([dataVarIdx1;    dataVarIdx1+1; ...
                             dataVarIdx1+2;  dataVarIdx1+3],1,[]);
    end
    switch dataOut.DataBits(i)
        case 8
            dataVal=cast(typecast(data(dataVarIdx1+dataOffset),...
                             dataOut.DataType{i}),'double');
            dataOffset=dataOffset+1;
        case 16
            dataVal=cast(typecast(data(dataVarIdx2+dataOffset),...
                             dataOut.DataType{i}),'double');
            dataOffset=dataOffset+2;
        case 32
            dataVal=cast(typecast(data(dataVarIdx4+dataOffset),...
                             dataOut.DataType{i}),'double');
            dataOffset=dataOffset+4;
        otherwise
            fprintf(2,['Unsupported data width in item %d:' ...
                       '%d Bits - Skipping.\n'], ...
                      dataOut.DataItem(i),dataOut.DataBits(i));
            dataOffset=dataOut.DataBits(i)/8;
            continue;
    end
    dataVal=dataVal*dataOut.Factor(i)+dataOut.Offset(i);
    dataOut.Data{i}=dataVal';
end
clear dataOffset dataBlockId dataVarIdx1 dataVarIdx2 dataVarIdx4 dataVal; 
clear i data;
