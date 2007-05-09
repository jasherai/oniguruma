require 'rubygems'
require 'hoe'

class Hoe 
   # Dirty hack to eliminate Hoe from gem dependencies
   def extra_deps 
      @extra_deps.delete_if{ |x| x.first == 'hoe' }
   end
   
   # Dirty hack to package only the required files per platform
   def spec= s 
      if ENV['PLATFORM'] =~ /win32/
         s.files = s.files.reject! {|f| f =~ /extconf\.rb/}
      else
         s.files = s.files.reject! {|f| f =~ /win\//}
      end
      @spec = s
   end
end

version = /^== *(\d+\.\d+\.\d+)/.match( File.read( 'History.txt' ) )[1]

Hoe.new('oniguruma', version) do |p|
  p.rubyforge_name = 'oniguruma'
  p.author = ['Dizan Vasquez', 'Nikolai Lugovoi']
  p.email = 'dichodaemon@gmail.com'
  p.summary = 'Bindings for the oniguruma regular expression library'
  p.description = p.paragraphs_of('README.txt', 1 ).join('\n\n')
  p.url = 'http://oniguruma.rubyforge.org'
  if ENV['PLATFORM'] =~ /win32/
     p.lib_files = ["win/oregexp.so"]
     p.spec_extras[:require_paths] = ["win", "lib", "ext" ]
     p.spec_extras[:platform] = Gem::Platform::WIN32
  else
     p.spec_extras[:extensions] = ["ext/extconf.rb"]
  end
  p.rdoc_pattern = /^(lib|bin|ext)|txt$/
  p.changes = p.paragraphs_of('History.txt', 0).join("\n\n")
end


