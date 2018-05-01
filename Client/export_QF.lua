--- 루아용 인터페이스 모듈 자동 생성하는 스크립트
--- 입력파일 : zz_interface.h
--- 출력파일 : zz_api_define.inc zz_api_register.inc
---
--- ss checkout $/znzin11/engine/include/zz_api_define.inc $/znzin11/engine/include/zz_api_register.inc -yzho,XXXX -i-y
---
--- $Header: /Client/export_QF.lua 1     03-07-02 1:26p Icarus $

--- 파일을 읽어 interfaces 테이블에 정보 저장
function read_interfaces ( hin, hout )
    i = 1
    word = read( hin, "*w" )

    while ( word ) do
        if (word ~= "QF_SCRIPT") then
            word = read( hin, "*l" ) -- skip current line
        else
            local return_type
            local name
            return_type = read( hin, "*w" )
            if (not return_type) then break end

            name = read( hin, "*w" )
            if (not name) then break end

            interfaces[name] = {}

            interfaces[name].return_type = return_type

            word = read( hin, "*w" ) -- skip (
            if (not word) then break end

            word = read( hin, "*w" )
            if (not word) then break end

            interfaces[name].params = {}
            local param_index = 1
            
            while (word ~= ");") do
                -- extract out comma(,)
                interfaces[name].params[param_index] = gsub( word, ",", "" )
                word = read( hin, "*w" ) -- skip parameter name
                if (not word) then break end
                if (word == ");") then break end
                word = read( hin, "*w" ) -- read next
                if (not word) then break end
                param_index = param_index + 1
            end
            i = i + 1
        end -- end of if
        word = read( hin, "*w" ) -- read next
    end -- end of while
end

IN_FILENAME     = "event/Quest_FUNC.h"
OUT_FILENAME1 = "event/Quest_FUNC_DEF.inc"
OUT_FILENAME2 = "event/Quest_FUNC_REG.inc"

hin   = readfrom ( IN_FILENAME   ) -- input file handle
hout1 = writeto  ( OUT_FILENAME1 ) -- output file handle
hout2 = writeto  ( OUT_FILENAME2 ) -- output file handle

interfaces = {} -- 인터페이스 정보 테이블

--- 인터페이스 정보 읽어들이기
read_interfaces ( hin, hout )

names = {} --- 정렬용 이름 테이블
i = 1
index = next(interfaces, nil) -- 이름으로 참조하는 테이블이므로, interfaces[i] 를 사용할 수 없다.
while (index) do
    names[i] = index
    index = next(interfaces, index)
    i = i + 1
end

--- 이름순으로 정렬하기
sort( names )

--- 매크로 참조 테이블들
map_return_types = {
    void    = "RETURNS_NONE",
    int     = "RETURNS_INT",
    float   = "RETURNS_FLOAT",
    HNODE   = "RETURNS_HNODE",
    ZSTRING = "RETURNS_STRING"
}
map_param_types = {
    void    = "",
    int     = "ZL_INT",
    float   = "ZL_FLOAT",
    HNODE   = "ZL_HNODE",
    ZSTRING = "ZL_STRING"
}

--- 경고 메세지
message_autogen =
"//==========================================================================//\n"..
"// 이 파일은 export_interface.lua 스크립트에 의해 자동생성된 코드입니다.\n"..
"// 직접 수정하지 마십시오.\n"..
"// "..date().."\n"..
"//==========================================================================//\n\n\n"

write( hout1, message_autogen )
write( hout2, message_autogen )

--- 스크립트 인터페이스 정의용 파일 생성하기
for i = 1, getn(names) do
    definition = format( "ZL_FUNC_BEGIN( %s, ", names[i] )
    -- export return_type
    definition = definition..format( "%s", map_return_types[ interfaces[names[i]].return_type ] )
    definition = definition.." )\n"
    
    for j = 1, getn(interfaces[names[i]].params) do
        if (j == getn(interfaces[names[i]].params)) then
            definition = definition..format( "\t%15s\n", map_param_types[ interfaces[names[i]].params[j] ] )
        else
            definition = definition..format( "\t%15s,\n", map_param_types[ interfaces[names[i]].params[j] ] )
        end
    end

    definition = definition.."ZL_FUNC_END\n\n"
        
    write( hout1, definition )
end

--- 스크립트 인터페이스 등록용 파일 생성하기
for i = 1, getn(names) do
    register = format( "ZL_REGISTER( %-30s )\n", names[i] )
    write( hout2, register )
end

--- 파일의 끝 표시
message_end_of_file = "// end of file\n"

write( hout1, message_end_of_file )
write( hout2, message_end_of_file )

closefile( hin   )
closefile( hout1 )
closefile( hout2 )

