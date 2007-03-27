require 'rubygems'
require 'hoe'

class Hoe; def extra_deps; @extra_deps.reject { |x| Array(x).first == 'hoe' }; end end

Hoe.new('oniguruma', '1.0.0') do |p|
  p.rubyforge_name = 'oniguruma'
  p.author = 'Dizan Vasquez'
  p.email = 'dix_ans@yahoo.com'
  p.summary = 'Bindings for the oniguruma regular expression library'
  p.description = p.paragraphs_of('README.txt', 1 ).join('\n\n')
  p.url = 'http://oniguruma.rubyforge.org'
  p.spec_extras[:extensions] = ["ext/extconf.rb"]
  p.rdoc_pattern = /^(lib|bin|ext)|txt$/
  p.changes = p.paragraphs_of('History.txt', 0).join("\n\n")
end


