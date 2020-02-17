function Channel=importfamos(FullRawData)
%__________________________________________________________________________
% The sequnce importfamos.m was produced to convert imc raw data(*.raw;
% *.dat) to matlab data. Here, struct is used to manage the channel
% information.
%
% For more information of FAMOS file format, please see the
% manufacturer's website: http://www.imc-berlin.de
%
% Corresponding to Data Structure in Matlab, the channels are stored
% as struct struct: Channel + name (channel name)
%                           + comment 
%                           + data (channel Value) 
%                           + length 
%                           + yUnit 
%                           + t0 
%                           + dt(sampling period) 
%                           + xUnit
% Version history:
% Version 1.0 (2011.1.19); Current version only can deal with analog rawdata
% from imc devices. The digital and group function is ongoning and will be
% released in version 2.
% Version 1.1 (2013.12.31): In order to solve non-manually save data,
% regular pattern is introduced into sloving data structure without CR and
% LF.
% (only support data(Channel) from imc devices)
%
%%-------------------------------------------------------------------------
% Author: Liang
% Danke.Liang@gmail.com
% Started on Dec.14, 2010
%__________________________________________________________________________
if nargin == 0;
  [RawData,FamosPath] = uigetfile({'*.raw';'*.dat'},'Select Famos Raw data');
  FullRawData=strcat(FamosPath,RawData);
end
fid = fopen(FullRawData,'r');
if fid==-1,
    disp('failed to read rawdata')
    return
end
disp(['Info: Read data @' datestr(now) ' from' ])
disp([ ' ' FullRawData '...'])
Textall=fscanf(fid,'%c');
CSstring='\|CS,\d,\D*\d+,\D*\d+,';
CSend=regexp(Textall,CSstring,'end');
InfoSegment=Textall(1,1:CSend); 
Cbstr='\|Cb.*?;';
CGstr='\|CG,.*?;';
CDstr='\|CD,.*?;';
NTstr='\|NT,.*?;';
CPstr='\|CP,.*?;';
CRstr='\|CR,.*?;';
CNstr='\|CN,.*?;';
ArrCG=char(regexp(InfoSegment,CGstr,'match'));
ChannelNum=size(ArrCG,1);
ArrCN=char(regexp(InfoSegment,CNstr,'match'));
ArrCD=char(regexp(InfoSegment,CDstr,'match'));
ArrNT=char(regexp(InfoSegment,NTstr,'match'));
ArrCP=char(regexp(InfoSegment,CPstr,'match'));
ArrCb=char(regexp(InfoSegment,Cbstr,'match'));
ArrCR=char(regexp(InfoSegment,CRstr,'match'));
%% CN
ChannelName=cell(ChannelNum,1);
ChannelComment=cell(ChannelNum,1);
%% CD,NT
CDsample=zeros(ChannelNum,1);
CDUnit=cell(ChannelNum,1);
TriggerTime=cell(ChannelNum,1);
%% CP
KeyBufferRef=zeros(ChannelNum,1);
KeyBytes=zeros(ChannelNum,1);
KeyNumberFormat=cell(ChannelNum,1);
KeySignBits=zeros(ChannelNum,1);
%% Cb
KeyBufferRefIndex=zeros(ChannelNum,1);
KeyBufferRefCb=zeros(ChannelNum,1);
KeyOffsetBufferInSamplesKey=zeros(ChannelNum,1);
KeyBufferFilledBytes=zeros(ChannelNum,1);
%% CR
KeyTransformation=zeros(ChannelNum,1);
KeyCRfactor=zeros(ChannelNum,1);
KeyCRoffset=zeros(ChannelNum,1);
KeyUnit=cell(ChannelNum,1);
%% Define Return object
Channel=struct('name','','comment','','data',[],'length',0,'yUnit','','t0','','dt','','xUnit','');
BinaryStart=CSend;
ChannelID=1;
while ChannelID <= ChannelNum
   temptext=char(ArrCD(ChannelID,:));  
   [CDsample(ChannelID,1), CDUnit{ChannelID,1}]=ProcessCD(temptext);
   [ChannelName{ChannelID,1},ChannelComment{ChannelID,1}]=ProcessCN(ArrCN(ChannelID,:));
   Channel(ChannelID).name=ChannelName{ChannelID,1};
   disp(strcat('Channel_',num2str(ChannelID),':',Channel(ChannelID).name))
   Channel(ChannelID).comment=ChannelComment{ChannelID,1};
   Channel(ChannelID).dt=strcat(num2str(CDsample(ChannelID,1)),CDUnit{ChannelID,1});
   Channel(ChannelID).xUnit=CDUnit{ChannelID,1};
   TriggerTime{ChannelID,1}=ProcessNT(ArrNT(ChannelID,:));
   Channel(ChannelID).t0=TriggerTime{ChannelID,1};
   [KeyBufferRef(ChannelID,1),KeyBytes(ChannelID,1),KeyNumberFormat{ChannelID,1},KeySignBits(ChannelID,1)]=ProcessCP(ArrCP(ChannelID,:));
   [KeyBufferRefIndex(ChannelID,1),KeyBufferRefCb(ChannelID,1),KeyOffsetBufferInSamplesKey(ChannelID,1),KeyBufferFilledBytes(ChannelID,1)]=ProcessCblittle(ArrCb(ChannelID,:));
   [KeyTransformation(ChannelID,1),KeyCRfactor(ChannelID,1),KeyCRoffset(ChannelID,1),KeyUnit{ChannelID,1}]=ProcessCR(ArrCR(ChannelID,:));
   Channel(ChannelID).yUnit=KeyUnit{ChannelID,1};
   BinaryRead= BinaryStart+KeyOffsetBufferInSamplesKey(ChannelID,1);
   ChannelLength=KeyBufferFilledBytes(ChannelID,1)*8/KeySignBits(ChannelID,1);
   Channel(ChannelID).data=ReadChannel(fid,BinaryRead,ChannelLength,KeyNumberFormat{ChannelID,1},KeyCRfactor(ChannelID,1),KeyCRoffset(ChannelID,1));
   Channel(ChannelID).length=ChannelLength;
   ChannelID=ChannelID+1;
end
fclose(fid);
end
%%
function [KeyDx, KeyUnit]= ProcessCD(TxtString)
% disp('Info: Processing key CD...');
CommaLocation=find(TxtString==',');
Txtemp=TxtString(CommaLocation(3)+1:CommaLocation(4)-1);
KeyDx=str2double(Txtemp);
KeyUnit=TxtString(CommaLocation(6)+1:CommaLocation(7)-1);
% disp('Info: Finished Process key CD!');
end
function TimeStart = ProcessNT(TxtString)
    
    CommaLocation=find(TxtString==',');
    Txtemp=TxtString(CommaLocation(3)+1:CommaLocation(4)-1);
    KeyDay=str2num(Txtemp);
    Txtemp=TxtString(CommaLocation(4)+1:CommaLocation(5)-1);
    KeyMonth=str2num(Txtemp);
    Txtemp=TxtString(CommaLocation(5)+1:CommaLocation(6)-1);
    KeyYear=str2num(Txtemp);
    
    Txtemp=TxtString(CommaLocation(6)+1:CommaLocation(7)-1);
    KeyHours=str2num(Txtemp);
    Txtemp=TxtString(CommaLocation(7)+1:CommaLocation(8)-1);
    KeyMinutes=str2num(Txtemp);
    Txtemp=TxtString(CommaLocation(8)+1:length(TxtString));
    KeySeconds=str2num(Txtemp);
    
    TimeStart=datestr(datenum([KeyYear, KeyMonth,KeyDay,KeyHours,KeyMinutes,KeySeconds]),'yyyy-mm-dd HH:MM:SS');
% disp('Info: Finished Processing key NT!');
end
function [KeyBufferRef,KeyBytes,KeyNumerFormat,KeySignBits]= ProcessCP(TxtString)
% disp('Info: Processing key CP...');
CommaLocation=find(TxtString==',');  
Txtemp=TxtString(CommaLocation(3)+1:CommaLocation(4)-1);
KeyBufferRef=str2num(Txtemp);
Txtemp=TxtString(CommaLocation(4)+1:CommaLocation(5)-1);
KeyBytes=str2num(Txtemp);
Txtemp=TxtString(CommaLocation(5)+1:CommaLocation(6)-1);
NumberFormat=str2num(Txtemp);
switch (NumberFormat)
    case 1
        KeyNumerFormat='*uint';
    case 2
        KeyNumerFormat='*int';
    case 3
        KeyNumerFormat='*ushort'; 
    case 4
        KeyNumerFormat='*short';
    case 5
        KeyNumerFormat='*ulong'; 
    case 6
        KeyNumerFormat='*long';
    case 7
        KeyNumerFormat='*float'; 
    case 8
        KeyNumerFormat='*float32';
    case 9
        KeyNumerFormat='*'; % imc Device Transitional Recording
    case 10
       KeyNumerFormat='*TimeStampASII' ;% TimeStamp is famos type
    case 11 
       KeyNumberFormat='*bit16'; %2-byte-word digital
    case 13
       KeyNumberFormat='*bit48';      
end
Txtemp=TxtString(CommaLocation(6)+1:CommaLocation(7)-1);
KeySignBits=str2num(Txtemp);
end
function [KeyBufferRefIndex,KeyBufferRefCb,KeyOffsetBufferInSamplesKey,KeyBufferFilledBytes] = ProcessCblittle(TxtString)
% disp('Info: Processing key Cb...');
CommaLocation=find(TxtString==',');
Txtemp=TxtString(CommaLocation(3)+1:CommaLocation(4)-1);
KeyBufferRefIndex=str2num(Txtemp);
Txtemp=TxtString(CommaLocation(5)+1:CommaLocation(6)-1);
KeyBufferRefCb=str2double(Txtemp);
Txtemp=TxtString(CommaLocation(7)+1:CommaLocation(8)-1);
KeyOffsetBufferInSamplesKey=str2double(Txtemp);
Txtemp=TxtString(CommaLocation(10)+1:CommaLocation(11)-1);
KeyBufferFilledBytes=str2double(Txtemp);
%disp('Info: Finished Processing key Cb!');
end
function [KeyTransformation,KeyCRfactor,KeyCRoffset,KeyUnit]= ProcessCR(TxtString)
%   disp('Info: Processing key CR...')
%
CommaLocation=find(TxtString==','); 
Txtemp=TxtString(CommaLocation(3)+1:CommaLocation(4)-1);
KeyTransformation=str2num(Txtemp);
Txtemp=TxtString(CommaLocation(4)+1:CommaLocation(5)-1);
KeyCRfactor=str2double(Txtemp);
Txtemp=TxtString(CommaLocation(5)+1:CommaLocation(6)-1);
KeyCRoffset=str2double(Txtemp);
Txtemp=TxtString(CommaLocation(7)+1:CommaLocation(8)-1);
KeyUnitLength=str2double(Txtemp);
KeyUnit=TxtString(CommaLocation(8)+1:CommaLocation(8)+KeyUnitLength);
%   disp('Info: Finished Processing key CR!');
end
function [ChannelName,ChannelComment]= ProcessCN(TxtString)
CommaLocation=find(TxtString==',');
ChannelName=TxtString(CommaLocation(7)+1:CommaLocation(8)-1);
ChannelCommLength=TxtString(CommaLocation(8)+1:CommaLocation(9)-1);
if ChannelCommLength=='0';
ChannelComment='';
else 
    temp=str2double(ChannelCommLength);
    ChannelComment=TxtString(CommaLocation(9)+1:CommaLocation(9)+temp);
end
end
function tempChannel=ReadChannel(FileID, ReadStart,ChannelLength, Datatype,factor,offset)
fseek(FileID,ReadStart,'bof');
tempChannel=double(fread(FileID,ChannelLength,Datatype))*factor+offset;
%disp('Info: a Channle was imported.... ');
end
