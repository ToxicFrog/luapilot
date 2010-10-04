local data = {
	boolean = { true, false };
	number = { 0, 1, -1, math.huge, -math.huge, 1/0, 0/0 };
	string = { "foo", "bar", "a rather long string with a bunch of characters in it", "" };
	["nil"] = { n=1, nil };
	table = {
		{ 1, 2, 3, 4 };
		{ { true }; { false }; };
		{ foo = { 1, 2, 3 }; bar = { 4, 5, 6 }; };
	};
	["function"] = {
		function() return 0 end;
		function() return true end;
	};
	--thread = not supported
	--userdata = not supported
	--function = not yet
}

local function pingpong(self)
	local cin,cout = self.cin,self.cout
	while true do
		local msg = cin:read()
		if msg == "done" then return true end
		cout:write(msg)
	end
end

local function eq(x, y)
	if x ~= x then -- NaN
		return y ~= y
	end
	
	if type(x) == "table" then
		for k,v in pairs(x) do
			if not eq(v, y[k]) then return false end
		end
		return true
	end
	
	if type(x) == "function" then
		return x() == y()
	end
	
	return x == y
end

for type,values in pairs(data) do
	for _,proc in pairs(tdata.processes) do
		proc.cin:write(pingpong)
		for i=1,(values.n or #values) do
			local val = values[i]
			proc.cin:write(values[i])
			local res = proc.cout:read()
			test("Bounce "..type.." via "..proc:getName(), eq(val, res))
		end
		proc.cin:write("done")
		proc.cout:read()
	end
end
