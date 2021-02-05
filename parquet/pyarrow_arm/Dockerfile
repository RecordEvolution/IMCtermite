
FROM ubuntu:19.10

RUN apt-get update -y && apt-get install -y \
	apt-utils \
	git g++ \
	make cmake \
	pkg-config \
	#build-essentials \
	python3 \
	python3-setuptools \
	cython3 \
	python3-numpy

RUN git clone https://github.com/apache/arrow.git --single-branch --depth=1

COPY . ./

RUN chmod u+x ./build_arrow_cpp.sh
RUN chmod u+x ./build_arrow_python.sh
RUN ./build_arrow_cpp.sh
RUN ./build_arrow_python.sh

#RUN chmod u+x ./build_arrow.sh
#CMD ["./build_arrow.sh"]
CMD ["sleep 1d"]
