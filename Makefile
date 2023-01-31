SOURCE:=$(shell cd morphy && find src | grep cpp$)
HEADER:=$(shell cd morphy && find include | grep hpp$)
CODE=${SOURCE} ${HEADER}

CLANG_FORMAT=clang-format-15 -style=file ${CODE}

CLANG_TIDY=clang-tidy-15 -p . ${CODE} -checks=llvm-header-guard
CLANG_TIDY_EXTRA= \
	-Iinclude \
	-I../godot-cpp/include \
	-I../godot-cpp/gen/include \
	-I../godot-cpp/gdextension \
	-std=c++17

check-format:
	docker-compose run clang sh -c "${CLANG_FORMAT} --dry-run -Werror"
	docker-compose run clang sh -c "${CLANG_TIDY} -- ${CLANG_TIDY_EXTRA}"

format:
	docker-compose run clang sh -c "${CLANG_TIDY} -fix -fix-errors -- ${CLANG_TIDY_EXTRA}"
	docker-compose run clang sh -c "${CLANG_FORMAT} -i"
