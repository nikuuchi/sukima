#!/usr/bin/env ruby

Dir.glob("*.lisp").each do |x|
    print `../lisp #{x}`
end
