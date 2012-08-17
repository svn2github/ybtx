gac_require "relation/cofc/CofcBaseInc"
--------------------------------------------------------------
function CCofcBase:CreateCofc(cofcName)
  Gac2Gas:CreateCofc(g_Conn, cofcName)
end
