SOURCE:=$(shell find src/morphy | grep cpp$)
HEADER:=$(shell find include/morphy | grep hpp$)
CODE=${SOURCE} ${HEADER}

CLANG_FORMAT=clang-format-15 -style=file ${CODE}

CLANG_TIDY=clang-tidy-15 -p . ${CODE} -checks=llvm-header-guard
CLANG_TIDY_EXTRA= \
	-Iinclude \
	-Iinclude/morphy \
	-Igodot-cpp/include \
	-Igodot-cpp/gen/include \
	-Igodot-cpp/gdextension \
	-std=c++17

check-format:
	docker-compose run clang sh -c "${CLANG_FORMAT} --dry-run -Werror"
	docker-compose run clang sh -c "${CLANG_TIDY} -- ${CLANG_TIDY_EXTRA}"

format:
	docker-compose run clang sh -c "${CLANG_TIDY} -fix -fix-errors -- ${CLANG_TIDY_EXTRA}"
	docker-compose run clang sh -c "${CLANG_FORMAT} -i"
