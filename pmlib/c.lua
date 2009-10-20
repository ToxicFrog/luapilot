CC = "gcc"
CCFLAGS = { "-Wall", "-c", "-std=c99" }
INCLUDES = EMPTY
COMPILE = "%CC% %CCFLAGS% %INCLUDES% -o %out[1]% %in[1]%"

LD = "%CC%"
LDFLAGS = EMPTY
LIBS = EMPTY
LINK = "%LD% %LDFLAGS% -o %out% %in% %LIBS%"


cfile = simple {
    class = "cfile";
    command = { "%COMPILE%" };
    outputs = { "%U%-%I%.o" };

    __init = function(self, f)
        if type(f) == 'string' then
            f = (SRCDIR or "")..f
        end

        return simple.__init(self, f)
    end;
}

lib = simple {
    class = "lib";
    command = {
        "ar rcu %out% %in%";
        "ranlib %out%";
    };
    outputs = { "%U%-%I%.a" };
}

sharedlib = simple {
    class = "sharedlib";
    command = { "%LINK%" };
    LDFLAGS = { PARENT, "-shared" };
    outputs = { "%U%-%I%.so" };
}

program = simple {
    class = "program";
    command = { "%LINK%" };
    outputs = { "%U%-%I%" };
}
