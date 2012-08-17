
<?php

class RequestManage
{
	private $CheckTemp   = "CheckSum.tmp";
	private $EncryptSeed = "";
	private $EncryptHead = "";
	private $EncryptTail = "";

	private $GatewayID   = 0;
	private $GameID      = 508;

	private $RequestList = array();

	private $CMD_BIND = 0x10000001;
	private $CMD_BIND_RES = 0x20000001;
	private $CMD_USER_SERIAL_QUERY = 0x10003206;
	private $CMD_USER_SERIAL_QUERY_RES = 0x20003206;


	function __construct()
	{
	}

	public function Response()
	{
		echo $_GET["data"];
	}

	private function RequestBind($Socket)
	{
		echo "==================== request bind ====================\n";
		// send connect request
		$TotalSize = 100;
		$PDU = array();
		$idx = 0;
		$PDU[$idx++] = pack("n", $TotalSize); // Total_Length UINT2
		$PDU[$idx++] = pack("C", 0x10); // Version UINT1
		$PDU[$idx++] = pack("C", 0x0); // Remain_Packages UINT1
		$PDU[$idx++] = pack("N", $this->CMD_BIND); // Command_ID UINT4
		$PDU[$idx++] = pack("N", 0x1); // Sequence_ID UINT4

		$Gateway_Code = "gw601001";
		$Gateway_Password = "dwlb0376aoyi4didp4adqk8unrp4y38i";
		$MAC = "002421305F5B";
		$PDU[$idx++] = pack("a32", $Gateway_Code); // Gateway_Code String[32]
		$PDU[$idx++] = pack("a32", $Gateway_Password); // Gateway_Password String[32]
		$PDU[$idx++] = pack("a12", $MAC); // MAC String[12]
		$PDU[$idx++] = pack("c", 0x0); // Reconnect_Flag INT1
		$PDU[$idx++] = pack("c", 0x1); // Pad INT1
		$PDU[$idx++] = pack("n", 0x2); // Pad UINT2
		$PDU[$idx++] = pack("N", 101001); // Server_ID UINT4

		// 获得校验位
		$Content = implode("", $PDU);
		$fp = fopen($this->CheckTemp, "wb");
		fwrite($fp, $Content, $TotalSize-4);
		fclose($fp);

		$Result = exec("./crc $this->CheckTemp");
		//

		$PDU[$idx++] = pack("n", 0x0); // Resv UINT2
		$PDU[$idx++] = pack("n", $Result); // Checksum UINT2

		$Request = implode("", $PDU);
		$uSize = socket_write($Socket, $Request, $TotalSize);

		$this->ResponseBind($Socket);
	}

	private function ResponseBind($Socket)
	{
		// crc16检验
		$fp = fopen($this->CheckTemp, "wb");

		$Buffer = socket_read($Socket, 2);
		fwrite($fp, $Buffer, 2);
		$Response = unpack("nTotal_Length", $Buffer);
		$LeaveSize = $Response['Total_Length'] - 2;

		$Buffer = socket_read($Socket, $LeaveSize);
		fwrite($fp, $Buffer, $LeaveSize-4); // 排除检验位
		$Response = unpack("CVersion/CRemain_Packages/NCommand_ID/NSequence_ID/NResult_Code/NGateway_ID/nResv/nChecksum", $Buffer);
		$this->GatewayID = $Response['Gateway_ID'];

		fclose($fp);
		$Result = exec("./crc $this->CheckTemp");
		#echo "$Result\n";

		print_r($Response);
	}

	private function RequestUserInfo($Socket)
	{
		echo "==================== request user info ====================\n";
		// send ID information Request
		$TotalSize = 32;
		$PDU = array();
		$idx = 0;
		$PDU[$idx++] = pack("n", $TotalSize); // Total_Length UINT2
		$PDU[$idx++] = pack("C", 0x10); // Version UINT1
		$PDU[$idx++] = pack("C", 0x0); // Remain_Packages UINT1
		$PDU[$idx++] = pack("N", $this->CMD_USER_SERIAL_QUERY); // Command_ID UINT4
		$PDU[$idx++] = pack("N", 0x2); // Sequence_ID UINT4

		$PDU[$idx++] = pack("n", $this->GameID); // Game_ID UINT2
		$PDU[$idx++] = pack("n", 0x0); // Pad UINT2
		$PDU[$idx++] = pack("N", $this->GatewayID); // Gateway_ID UINT4
		$PDU[$idx++] = pack("N", 1000016); // User_ID UINT4
		$PDU[$idx++] = pack("N", 2009122501); // Activity_ID UINT4

		// 获得校验位
		$Content = implode("", $PDU);
		$fp = fopen($this->CheckTemp, "wb");
		fwrite($fp, $Content, $TotalSize-4);
		fclose($fp);

		$Result = exec("./crc $this->CheckTemp");
		//

		$PDU[$idx++] = pack("n", 0x0); // Resv UINT2
		$PDU[$idx++] = pack("n", $Result); // Checksum UINT2

		$Request = implode("", $PDU);
		socket_write($Socket, $Request, $TotalSize);

		$this->ResponseUserInfo($Socket);
	}

	private function ResponseUserInfo($Socket)
	{
		// crc16检验
		$fp = fopen($this->CheckTemp, "wb");

		$Buffer = socket_read($Socket, 2);
		fwrite($fp, $Buffer, 2);
		$Response = unpack("nTotal_Length", $Buffer);
		$LeaveSize = $Response['Total_Length'] - 2;

		$Buffer = socket_read($Socket, $LeaveSize);
		fwrite($fp, $Buffer, $LeaveSize-4); // 排除检验位
		$Response = unpack("CVersion/CRemain_Packages/NCommand_ID/NSequence_ID/NResult_Code/NUse_Time/nResv/nChecksum", $Buffer);

		fclose($fp);
		$Result = exec("./crc $this->CheckTemp");
		#echo "$Result\n";

		print_r($Response);
		//print_r(localtime($Response['Use_Time'], true));
	}

	public function DataOperate()
	{
		$Host = "192.168.41.89"; // 192.168.43.138
		$Port = 5091;
		$Proto = getprotobyname("tcp");

		// simple client
		$Socket = socket_create(AF_INET, SOCK_STREAM, $Proto) or die("Create socket failed!!");
		$Result = socket_connect($Socket, $Host, $Port) or die("Can not connect to server!!");

		$this->RequestBind($Socket);
		$this->RequestUserInfo($Socket);

		socket_close($Socket);
	}

	public function Check()
	{
		if (strlen($this->EncryptSeed) >= 8 && strlen($this->EncryptHead) >= 4)
		{
			$HeadFlag = "";
			$SeedStr = &$this->EncryptSeed;
			$HeadStr = &$this->EncryptHead;
			$HeadFlag .= $SeedStr[7] ^ $HeadStr[3];
			$HeadFlag .= $SeedStr[6] ^ $HeadStr[2];
			$HeadFlag .= $SeedStr[5] ^ $HeadStr[1];
			$HeadFlag .= $SeedStr[4] ^ $HeadStr[0];
			if ( $HeadFlag != 'YBTX' )
				return false;

			return true;
		}

		return false;
	}

	public function Analysis()
	{
		$Buffer = &$_GET["data"];
		$Len = strlen($Buffer);
		$StrList = array();
		for ($i=0; $i<$Len; ++$i)
		{
			$ch = $Buffer[$i];

			switch ($ch)
			{
			case 'i':
				// 获取数值
				++$i;
				while ( $i<$Len && ($ch = $Buffer[$i]) != 'e' )
				{
					array_push($StrList, $ch);
					++$i;
				}
				$this->EncryptSeed = implode('', $StrList);
				$StrList = array();

				break;
			case 'h':
				// 获取哈希键值对
				$KeyStr = "";
				$ValueStr = "";
				// 获取键
				do {
					$Number = 0;
					++$i;
					while ( $i<$Len && ($ch = $Buffer[$i]) != ':' )
					{
						$Number = ord($ch) - 48 + $Number * 10;
						++$i;
					}

					$KeyStr = substr($Buffer, $i+1, $Number);
					$i += $Number;

					$Number = 0;
					++$i;
					while ( $i<$Len && ($ch = $Buffer[$i]) != ':' )
					{
						$Number = ord($ch) - 48 + $Number * 10;
						++$i;
					}

					$ValueStr = substr($Buffer, $i+1, $Number); 
					$i += $Number;

					$this->RequestList[$KeyStr] = $ValueStr;
				} while ( $i+1<$Len && $Buffer[$i+1] != 'e');

				break;
			case 's':
				// 获取字串
				$Number = 0;
				++$i;
				while ( $i<$Len && ($ch = $Buffer[$i]) != ':' )
				{
					$Number = ord($ch) - 48 + $Number * 10;
					++$i;
				}

				if ( $this->EncryptHead == "" )
					$this->EncryptHead = substr($Buffer, $i+1, $Number);
				else
					$this->EncryptTail = substr($Buffer, $i+1, $Number);

				$i += $Number;
			}
		}

		// 验证请求数据包
		if ( $this->Check() )
		{
			$this->DataOperate(); // 调用数据库RPC操作，获取激活码账号的相关资料
			$this->Response();    // 封装请求回复信息
		}
	}
}


$reqMgr = new RequestManage();
#$reqMgr->Analysis();
$reqMgr->DataOperate();



?>

