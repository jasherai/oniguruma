require 'rubygems'
require 'hoe'

rubyforge_name = "oniguruma"

begin

   class Hoe 
      # Dirty hack to eliminate Hoe from gem dependencies
      def extra_deps 
         @extra_deps.delete_if{ |x| x.first == 'hoe' }
      end
   end
   
   h = Hoe.spec('oniguruma') do |p|
      p.rubyforge_name = 'oniguruma'
      p.version = /^== *(\d+\.\d+\.\d+)/.match( File.read( 'History.txt' ) )[1]
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
      p.changes = p.paragraphs_of('History.txt', 0).join("\n\n")
      p.clean_globs = ["manual/*"]
   end
   
   h.spec.extra_rdoc_files = h.spec.files.grep('/txt/$')
   
   if ENV['PLATFORM'] =~ /win32/
     h.spec.files -=  Dir["ext/extconf.rb"]
   else
     h.spec.files -= Dir["win/oregexp.so"]
   end

   desc 'Create MaMa documentation'
   task :mama => :clean do
      system "mm -c -t refresh -o manual mm/manual.mm"
   end
   
   desc 'Publish MaMa documentation to RubyForge'
      task :mama_publish => [:clean, :mama] do
      config = YAML.load(File.read(File.expand_path("~/.rubyforge/user-config.yml")))
      host = "#{config["username"]}@rubyforge.org"
      remote_dir = "/var/www/gforge-projects/#{h.rubyforge_name}"
      local_dir = 'manual'
      system "rsync -av #{local_dir}/ #{host}:#{remote_dir}"
   end

rescue LoadError => e
   desc 'Run the test suite.'
   task :test do
      system "ruby -Ibin:lib:test test_#{rubyforge_name}.rb"
   end
end


