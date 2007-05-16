~Oniguruma for Ruby~
~~Ruby bindings for oniguruma~~

#Main# | index

= About =

This library provides bindings to [Oniguruma http://www.geocities.jp/kosako3/oniguruma/],
a regular expression library which is standard with Ruby 1.9. It allows users
of Ruby 1.8 to access the extended functionality without the need to recompile
or to upgrade Ruby.

This site provides an overview of the library, the api documentation is 
[here http://oniguruma.rubyforge.org/oniguruma].

= Requirements =

* [Oniguruma http://www.geocities.jp/kosako3/oniguruma/] regular expression library (/>= 4.x.x/)

= Installation =

If you have [rubygems http://docs.rubygems.org/] installation is straightforward by typing 
(as root if needed):

--------hl shell-unix-generic,,false------
gem install -r oniguruma
------------------------------------------

If you prefer to get the sources, the last stable version may be
downloaded [here http://rubyforge.org/frs/?group_id=3289].

= Status =

The library interface is mature. Existing bugs are being fixed as they are
found.

#Use#

= Details =

Oniguruma provides the same functionality (and almost the same interface) than
standard regexp in Ruby. The main missing feature is the slash syntax to
declare regular expressions `/regexp/`. On the other hand, oniguruma provides
additional functionality:

*   Extensive multibyte support.
    -----------hl ruby,,false-----
    reg = ORegexp.new( 'р(уби.*)', 'i', 'utf8' )
    matches = reg.match("Text: Ехал Грека Через Реку")
    puts matches[0]                    #=> "Ехал"
    ------------------------------

*   Named groups.
    -----------hl ruby,,false-----
    reg = ORegex.new( '(?<before>.*)(a)(?<after>.*)' )
    match = reg.match( 'terraforming' )
    puts match[0]                      #=> 'terraforming'
    puts match[:before]                #=> 'terr'
    puts match[:after]                 #=> 'forming'
    ------------------------------

*   Named backreferences.
    -----------hl ruby,,false-----
    re = ORegexp.new('(?<pre>\w+?)\d+(?<after>\w+)')
    puts re.sub('abc123def', ' \<after>123\<pre> ')   #=> " def123abc "
    ------------------------------

*   Positive and negative Lookbehind.
    -----------hl ruby,,false-----
    re = ORegexp.new('(?>!g)ong')
    m1 = re.match("song")
    puts m1[0]                         #=> ong
    m2 = re.match("gong")              #=> nil
    ------------------------------
    
*   Support for different regexp syntaxes (Python, Perl, etc.).
    -----------hl ruby,,false-----
    re = ORegexp.new( 'section{ (?P<name> [^}]* ) }', :syntax => SYNTAX_PYTHON )
    re.sub('section{First}', 'subsection{\1}')        #=> "subsection{First}"
    ------------------------------

# Links #


= Rubyforge project page =

* [Textpow http://rubyforge.org/projects/oniguruma].

= Documentation =

* [Api Documentation http://oniguruma.rubyforge.org/oniguruma].

= Requirements =

* [Oniguruma http://www.geocities.jp/kosako3/oniguruma/].

= Projects using Textpow =

* [Textpow http://textpow.rubyforge.org/].
* [Ultraviolet Syntax Highlighting Engine http://ultraviolet.rubyforge.org/].
* [Macaronic markup engine http://mama.rubyforge.org/].
