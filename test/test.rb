#!/usr/bin/env ruby

Dir.glob("*.lisp").each do |x|
    print `../Release/lisp #{x}`
end
