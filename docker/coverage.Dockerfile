### Generate test coverage ###
# Define builder stage
FROM bigbear:base as builder

# Share work directory
COPY . /usr/src/project
WORKDIR /usr/src/project/build_coverage

COPY --from=builder /usr/src/project/build/bin/server .
COPY --from=builder /usr/src/project/shell_tests .


# Build and test
RUN cmake -DCMAKE_BUILD_TYPE=Coverage ..
RUN make
RUN make test ARGS=-V
RUN make coverage

