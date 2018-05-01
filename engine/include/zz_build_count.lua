------------------------------------------------------------------------------------------------
---
--- $Header: /engine/include/zz_build_count.lua 5     04-04-26 7:39p Zho $
---
--- open build count file
--- read build counter
--- increase build counter
--- save build counter
---
--- 2004-02-28 zho created.
--- 2004-02-29 zho version string feature added.
------------------------------------------------------------------------------------------------

BUILD_COUNT_FILE = "zz_build_count"
VERSION_STRING_FILE = "zz_version_info.inc"
build_count = 0

------------------------------------------------------------------------------------------------
--- open and read build counter
------------------------------------------------------------------------------------------------
fp = io.open( BUILD_COUNT_FILE )
if ( fp ) then 
    io.input( fp )
    build_count = io.read( "*n" )
    io.close( fp )
end
    
------------------------------------------------------------------------------------------------
-- increase build counter
------------------------------------------------------------------------------------------------
if (build_count) then
    build_count = build_count + 1
else
    build_count = 0
end
--print( "build_count = "..tostring( build_count ) )

------------------------------------------------------------------------------------------------
-- make version string
------------------------------------------------------------------------------------------------
date_string = os.date( "*t" )
-- for engine log
version_string = string.format(  '%d.%d.%d.%d', date_string[ "year" ]-2004, date_string[ "month" ], date_string[ "day" ], build_count )
-- for resource script
version_string_comma = string.format(  '%d,%d,%d,%d', date_string[ "year" ]-2004, date_string[ "month" ], date_string[ "day" ], build_count )
print( "version_string = "..version_string )

------------------------------------------------------------------------------------------------
-- save build counter and version string
------------------------------------------------------------------------------------------------
fp = io.open( BUILD_COUNT_FILE, "w+" )
if ( fp ) then
    io.output( fp )
    io.write( tostring( build_count ) )
    io.close( fp )
end

fp = io.open( VERSION_STRING_FILE, "w+" )
if ( fp ) then
    io.output( fp )
    io.write( '// Do not edit this!\n' )
    io.write( '// This is automatically created by zz_build_count.lua script before the build.\n\n' )
    io.write( '#define ZZ_BUILD_COUNT ' )
    io.write( tostring( build_count )..'\n' )
    io.write( '#define ZZ_VERSION_NO '..version_string..'\n' )
    io.write( '#define ZZ_VERSION_STRING "'..version_string..'"\n\n' )
    io.write( '#define ZZ_VERSION_NO_COMMA '..version_string_comma..'\n' )
    io.write( '#define ZZ_VERSION_STRING_COMMA "'..version_string_comma..'"\n\n' )
    io.flush( fp )
    io.close( fp )
end

------------------------------------------------------------------------------------------------
-- time wait for update 
------------------------------------------------------------------------------------------------
function zz_sleep (num_second)
	local start_time = os.clock()	
	local end_time = os.clock()
	repeat
		end_time = os.clock()
	until (os.difftime( end_time, start_time ) > num_second)
end
---zz_sleep(2) -- time for compiler to notify that the version file was updated.
