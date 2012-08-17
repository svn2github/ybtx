<?php
require_once("../../conf/gm_tools/Config.php");
require_once("../../lib/gm_tools/Lib.php");

function Main(){
        echo Date("Y-m-d h:i:s")."==============================\n";
        global $tbl_gm_db,$tbl_serv_db,$tbl_game_server;
        $dbinfo = $tbl_gm_db[0];
        $Conn = mysql_connect($dbinfo[2], $dbinfo[0], $dbinfo[1]);
        mysql_select_db($dbinfo[3], $Conn);
        mysql_query("delete from tbl_rush_role_servers where rrs_dEndDate < curdate()") or print(mysql_error());
        $query = "select rrs_uServerId from tbl_rush_role_servers";
        $result = mysql_query($query) or print(mysql_error());
        while($row = mysql_fetch_row($result)){
                echo "Server: ".$row[0]."\t".$tbl_game_server[$row[0]][2]."\n";
                $dbinfo = $tbl_serv_db[$row[0]];
                $Conn = mysql_connect($dbinfo[2], $dbinfo[0], $dbinfo[1]);
                mysql_select_db($dbinfo[3], $Conn);
                $qs = "select sc_sVarValue from tbl_conf_server where sc_sVarName = 'MaxRushRoleNumPerCamp' and sc_sVarValue < (select ceil(sc_sVarValue)/3 from tbl_conf_server where sc_sVarName = 'MaxRushRoleNumPerServer')";
                $rs = mysql_query($qs) or print(mysql_error());
                if($r = mysql_fetch_row($rs)){
                        echo "MaxRushRoleNumPerCamp: ".$r[0]."\n";
                        $Camp = $r[0];
                }else{
                        echo "MaxRushRoleNumPerCamp > 1/3 MaxRushRoleNumPerServer.\n";
                        continue;
                }

                $qs = "select cs.cs_uCamp, count(*) c from tbl_char c, tbl_char_static cs where c.cs_uId = cs.cs_uId group by cs.cs_uCamp having c >= ".$Camp;
                $rs = mysql_query($qs) or print(mysql_error());
                if(mysql_num_rows($rs) < 2){
                        echo "Only [".mysql_num_rows($rs)."] Camp(s) with MaxRushRoleNumPerCamp.\n";
                        continue;
                }

                $qs = "select cs.cs_uCamp, count(*) c from tbl_char c, tbl_char_static cs where c.cs_uId = cs.cs_uId group by cs.cs_uCamp having c > ".($Camp-800);
                $rs = mysql_query($qs) or print(mysql_error());
                if(mysql_num_rows($rs) < 3){
                        echo "Only [".mysql_num_rows($rs)."] Camps with MaxRushRoleNumPerCamp - 800.\n";
                        continue;
                }

                echo "Need to change args:\n";

                $param = array();
                $param["name"] = "MaxRushRoleNumPerCamp";
                $param["value"] = $Camp + 500;
                echo "Number: ".$param["value"]."\n";
                echo "Result: ".PostData($tbl_game_server[$row[0]][0],$tbl_game_server[$row[0]][1],"SetConfServer",$param)."\n\n";
        }
}

Main();
?>