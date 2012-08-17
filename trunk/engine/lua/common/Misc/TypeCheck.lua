function IsNil(x)
  return type(x) == "nil"
end

function IsBoolean(x)
  return type(x) == "boolean"
end

function IsNumber(x)
  return type(x) == "number"
end

function IsString(x)
  return type(x) == "string"
end

function IsTable(x)
  return type(x) == "table"
end

function IsFunction(x)
  return type(x) == "function"
end

function IsThread(x)
  return type(x) == "thread"
end

function IsUserData(x)
  return type(x) == "userdata"
end