-- 파일이름 : QuestGlobal.lua
-- 파일용도 : 3Ddata/QUEST 폴더안의  lua 파일에서 전역사용자정의 함수를 선언함



-- 1차전직
function G_giveJob_1(jobCode)

  local level
  level = GF_getVariable( 3 ) 

  if (level < 10) then    return
  end

  if     (jobCode == 111) then   GF_setVariable ( 4, jobCode )
  elseif (jobCode == 211) then   GF_setVariable ( 4, jobCode )
  elseif (jobCode == 311) then   GF_setVariable ( 4, jobCode )
  elseif (jobCode == 411) then   GF_setVariable ( 4, jobCode )
  end
end

-- 2차전직


-- 3차전직











