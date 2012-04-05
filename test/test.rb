#!/usr/bin/env ruby

Dir.glob("test/*.lisp").each do |x|
    print `./lisp #{x}`
end
