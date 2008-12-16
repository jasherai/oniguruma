# -*- encoding: utf-8 -*-

Gem::Specification.new do |s|
  s.name = %q{oniguruma}
  s.version = "1.1.1"

  s.required_rubygems_version = Gem::Requirement.new(">= 0") if s.respond_to? :required_rubygems_version=
  s.authors = ["Dizan Vasquez", "Nikolai Lugovoi"]
  s.date = %q{2008-12-15}
  s.description = %q{Ruby bindings to the Oniguruma[http://www.geocities.jp/kosako3/oniguruma/] regular expression library (no need to recompile Ruby).}
  s.email = %q{dichodaemon@gmail.com}
  s.extensions = ["ext/extconf.rb"]
  s.extra_rdoc_files = ["History.txt", "Manifest.txt", "README.txt", "Syntax.txt"]
  s.files = ["History.txt", "Manifest.txt", "README.txt", "Syntax.txt", "Rakefile", "lib/oniguruma.rb", "ext/oregexp.c", "ext/extconf.rb", "test/test_oniguruma.rb"]
  s.has_rdoc = true
  s.homepage = %q{http://oniguruma.rubyforge.org}
  s.rdoc_options = ["--main", "README.txt"]
  s.require_paths = ["lib", "ext"]
  s.rubyforge_project = %q{oniguruma}
  s.rubygems_version = %q{1.3.1}
  s.summary = %q{Bindings for the oniguruma regular expression library}
  s.test_files = ["test/test_oniguruma.rb"]

  if s.respond_to? :specification_version then
    current_version = Gem::Specification::CURRENT_SPECIFICATION_VERSION
    s.specification_version = 2

    if Gem::Version.new(Gem::RubyGemsVersion) >= Gem::Version.new('1.2.0') then
      s.add_development_dependency(%q<hoe>, [">= 1.8.2"])
    else
      s.add_dependency(%q<hoe>, [">= 1.8.2"])
    end
  else
    s.add_dependency(%q<hoe>, [">= 1.8.2"])
  end
end
