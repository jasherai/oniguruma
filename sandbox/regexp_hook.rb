class << Regexp
   
   alias :oni_old_new :callcc
   
   def callcc *args
      puts "allocate(#{args})"
      Regexp.oni_old_new *args
   end
end

x = /abc/
y = Regexp.new('def')