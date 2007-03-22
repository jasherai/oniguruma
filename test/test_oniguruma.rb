require 'oniguruma'
require 'test/unit'


class ORegexpTestCase < Test::Unit::TestCase
   def test_initialization
      assert_nothing_raised do
         reg  = Oniguruma::ORegexp.new( "(3.)(.*)(3.)" )
      end
   end
   
   def test_compile
      assert_nothing_raised do
         reg  = Oniguruma::ORegexp.compile( "(3.)(.*)(3.)" )
      end
   end
   
   def test_escape
      assert_equal( '\\\\\*\?\{\}\.', Oniguruma::ORegexp.escape('\\*?{}.') ) 
   end
   
   def test_last_match
      assert_equal( 0, Oniguruma::ORegexp.new( 'c(.)t') =~ 'cat' )
      assert_equal( "cat", Oniguruma::ORegexp.last_match(0) )
      assert_equal( "a", Oniguruma::ORegexp.last_match(1) )
      assert_equal( nil, Oniguruma::ORegexp.last_match(2) )
   end

   def test_bad_initialization
      assert_raises(Exception) do
         reg  = Oniguruma::ORegexp.new( "(3.)(.*)(3.))" )
      end
   end
   
   def test_match
      reg  = Oniguruma::ORegexp.new( "(3.)(.*)(3.)" )
      assert_not_nil( reg.match( "12345634" ) )
   end
   
   def test_no_match
      reg  = Oniguruma::ORegexp.new( "(3.)(.*)(3.)" )
      assert_nil( reg.match( "12145614" ) )
   end
   
   def test_sub
      reg = Oniguruma::ORegexp.new( 'pe')
      assert_equal( "**nelope", reg.sub( 'penelope', '**' ) )
      assert_equal( "++nelope", reg.sub( 'penelope' ) { |m| '++' })
   end
   
   def test_gsub
      reg = Oniguruma::ORegexp.new( '\(\?#(\w+?)\)')
      string = 'My favorite fruits are (?#fruit1), (?#fruit2), and (?#fruit3)'
      assert_equal( "My favorite fruits are *, *, and *", reg.gsub( string, '*' ) )
      fruits = { "fruit1" => "apples", "fruit2" => "bananas", "fruit3" => "grapes" }
      assert_equal( "My favorite fruits are apples, bananas, and grapes", reg.gsub( string ) { |text, match| fruits[match[1]]} )
   end
   
   def test_eql
      assert_equal( Oniguruma::ORegexp.new( 'expression'),  Oniguruma::ORegexp.new( 'expression') )
      assert_not_equal( Oniguruma::ORegexp.new( 'expression'),  Oniguruma::ORegexp.new( 'expresssion') )
      assert_not_equal( Oniguruma::ORegexp.new( 'expression', :encoding => Oniguruma::ENCODING_ASCII ),  Oniguruma::ORegexp.new( 'expression', :encoding => Oniguruma::ENCODING_ISO_8859_1 ) )
      assert_not_equal( Oniguruma::ORegexp.new( 'expression', :options => Oniguruma::OPTION_IGNORECASE ),  Oniguruma::ORegexp.new( 'expression', :options => Oniguruma::OPTION_NONE ) )
   end
   
   def test_case_eql
      a = "HELLO"
      result = ""
      case a
      when Oniguruma::ORegexp.new('^[a-z]*$'); result =  "Lower case\n"
      when Oniguruma::ORegexp.new('^[A-Z]*$'); result =  "Upper case\n"
      else;                         result =  "Mixed case\n"
      end
      
      assert_equal( "Upper case\n", result )
   end
   
   def test_operator_match
      assert_equal( nil, Oniguruma::ORegexp.new( 'SIT' ) =~ "insensitive" )
      assert_equal( 5, Oniguruma::ORegexp.new( 'SIT', :options => Oniguruma::OPTION_IGNORECASE ) =~ "insensitive" )
   end
   
#    def test_operator_match_2
#       $_ = "input data"
#       assert_equal( 7, ~Oniguruma::ORegexp.new( 'at' ) )
#    end

   def test_inspect
      assert_equal( "/cat/im", Oniguruma::ORegexp.new( 'cat', :options => Oniguruma::OPTION_IGNORECASE | Oniguruma::OPTION_MULTILINE ).inspect )
   end

   def test_to_s
      assert_equal( "(?im-x)cat", Oniguruma::ORegexp.new( 'cat', :options => Oniguruma::OPTION_IGNORECASE | Oniguruma::OPTION_MULTILINE ).to_s )
   end
   
   def test_kcode
      reg  = Oniguruma::ORegexp.new( "(3.)(.*)(3.)" )
      assert_equal( Oniguruma::ENCODING_ASCII, reg.kcode )
   end
   
   def test_options
      assert_equal( 3, Oniguruma::ORegexp.new( 'abc', :options => Oniguruma::OPTION_IGNORECASE | Oniguruma::OPTION_EXTEND ).options )
   end
   
   def test_source
      string = '(?<=\n)\\.*ocatarinetabelachitchix'
      assert_equal( string, Oniguruma::ORegexp.new( string ).source )
   end
end

class MatchDataTestCase < Test::Unit::TestCase
   def setup
      @reg = Oniguruma::ORegexp.new( '(.)(.)(\d+)(\d)' )
   end
   
   def test_square_brackets
      matches = @reg.match( "THX1138." )
      assert_equal( "HX1138", matches[0] )
      assert_equal( ["H", "X"], matches[1, 2] )
      assert_equal( ["H", "X", "113"], matches[1..3] )
      assert_equal( ["X", "113"], matches[-3, 2] )
   end
   
   def test_begin
      matches = @reg.match( "THX1138." )
      assert_equal( 1, matches.begin(0) )
      assert_equal( 2, matches.begin(2) )
   end
   
   def test_captures
      matches = @reg.match( "THX1138." )
      assert_equal( ["H", "X", "113", "8" ], matches.captures )
   end
   
   def test_end
      matches = @reg.match( "THX1138." )
      assert_equal( 7, matches.end(0) )
      assert_equal( 3, matches.end(2) )
   end
   
   def test_size
      matches = @reg.match( "THX1138." )
      assert_equal( 5, matches.length )
      assert_equal( 5, matches.size )
   end
   
   def test_offset
      matches = @reg.match( "THX1138." )
      assert_equal( [1, 7], matches.offset(0) )
      assert_equal( [6, 7], matches.offset(4) )
   end
   
   def test_post_match
      matches = @reg.match( "THX1138: The Movie" )
      assert_equal( ": The Movie", matches.post_match )
   end
   
   def test_pre_match
      matches = @reg.match( "THX1138." )
      assert_equal( "T", matches.pre_match )
   end
   
   def test_select
      matches = @reg.match( "THX1138: The Movie" )
      assert_equal( ["HX1138", "113"], matches.select{ |v| v =~ /\d\d/} )
   end
   
   def test_string
      matches = @reg.match( "THX1138." )
      assert_equal( "THX1138.", matches.string )
      assert( matches.string.frozen? )
   end
   
   def test_to_a
      matches = @reg.match( "THX1138." )
      assert_equal( ["HX1138", "H", "X", "113", "8" ], matches.to_a )
   end
   
   def test_to_s
      matches = @reg.match( "THX1138." )
      assert_equal( "HX1138", matches.to_s )
   end
   
   def test_values_at
      matches = @reg.match( "THX1138: The Movie" )
      assert_equal( ["HX1138", "X", "113"], matches.values_at( 0, 2, -2) )
   end
   
   def test_match_all
      reg = Oniguruma::ORegexp.new( 'ca' )
      matches = reg.match_all( 'ocatacachaca' )
      assert_equal( 3, matches.size )
      assert_equal( 7, matches.position(2) )
      assert_equal( "ca", matches.string[matches.begin(1)...matches.end(1)])
   end
   
   def test_match_empty_string
      reg = Oniguruma::ORegexp.new( '^\s*?(\n|\r)', :options => Oniguruma::OPTION_MULTILINE )
      matches = reg.match( "\n\n\n\n\n" )
      assert_not_nil( matches )
      assert_equal( "\n\n\n\n", matches.post_match )
   end
   
   def test_group_by_name
      reg = Oniguruma::ORegexp.new( '(?<begin>\()(?<body>.*)(?<end>\))', :options => Oniguruma::OPTION_MULTILINE )
      matches = reg.match( "blah (content) blah" )
      assert_not_nil( matches )
      assert_equal( '(', matches[:begin] )
      assert_equal( 'content', matches[:body] )
      assert_equal( ')', matches[:end] )
      assert_equal( nil, matches[:inexistent])
   end
   
end
