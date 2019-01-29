### Generate test coverage ###
# Define builder stage
FROM bigbear:base as builder

# Share work directory
COPY . /usr/src/project
WORKDIR /usr/src/project/build

RUN cmake ..
RUN make

WORKDIR /usr/src/project/build_coverage

# Build and test
RUN cmake -DCMAKE_BUILD_TYPE=Coverage ..
RUN make coverage

