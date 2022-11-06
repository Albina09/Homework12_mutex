.PHONY := clean
bin := Project
sources := Project.c

$(bin): $(sources)
		gcc $(sources) -o $(bin) -pthread

clean:
		rm -rf $(bin)

