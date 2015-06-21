module(..., package.seeall)

function exec_and_read_line(exec)
  local cmd = io.popen("xcode-select -p")
  local output = string.gsub(cmd:read("*a"), "%s", "")
  cmd:close()

  return output
end
