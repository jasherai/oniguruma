== ONIGURUMA FOR RUBY:

Ruby bindings to the Oniguruma[http://www.geocities.jp/kosako3/oniguruma/] regular expression library (no need to recompile Ruby).

== FEATURES:
  
* Increased performance. 
* Same interface than standard Regexp class (easy transition!). 
* Support for named groups, look-ahead, look-behind, and other 
  cool features!
* Support for other regexp syntaxes (Perl, Python, Java, etc.)

== SYNOPSIS:

 reg = Oniguruma::ORegexp.new( '(?<before>.*)(a)(?<after>.*)' )
 match = reg.match( 'terraforming' )
 puts match[0]         <= 'terraforming'
 puts match[:before]   <= 'terr'
 puts match[:after]    <= 'forming'
 
== SYNTAX

Consult the Syntax.txt[link:files/Syntax_txt.html] page.

== REQUIREMENTS:

* Oniguruma[http://www.geocities.jp/kosako3/oniguruma/] library v. 4.6 or higher

== INSTALL:

sudo gem install -r oniguruma

== BUGS/PROBLEMS/INCOMPATIBILITIES:

* <code>ORegexp#~</code> is not implemented.
* <code>ORegexp#kcode</code> results are not compatible with <code>Regexp</code>.
* <code>ORegexp</code> options set in the string are not visible, this affects
  <code>ORegexp#options</code>, <code>ORegexp#to_s</code>, <code>ORegexp#inspect</code>
  and <code>ORegexp#==</code>.

== TODO:

* Complete documentation (methods, oniguruma syntax).

== CREDITS:

* N. Lugovoi. ORegexp.sub and ORegexp.gsub code and lots of other stuff.
* K. Kosako. For his great library.
* A lot of the documentation has been copied from the original Ruby Regex documentation.

== LICENSE:

New BSD License

Copyright (c) 2007, Dizan Vasquez
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the 
  documentation and/or other materials provided with the distribution.
* Neither the name of the author nor the names of its contributors may be used to endorse or promote products derived from this 
  software without specific prior written permission.
            
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
