
build :
	docker build . --tag pyarrowbuild

run :
	docker run -it pyarrowbuild:latest

run-bash :
	docker run -it --volume=$(pwd)/build:/home pyarrowbuild:latest /bin/bash

run-volume :
	docker run -it -v /home/pirate/pyarrow/build/:/arrow/python/ pyarrowbuild:latest
	#sudo docker run -it --volume=$(pwd)/build:/home ubuntu:latest /bin/bash

rm-container :
	cont=$(docker ps -a | tail -n 26 | awk '{print $NF}' | sed ':a;N;$!ba;s/\n/ /g')
	echo ${cont}
	docker rm ${cont}

rm-image :
	img=$(docker image ls --quiet | sed ':a;N;$!ba;s/\n/ /g')
	docker image rm ${img}

