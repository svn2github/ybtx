
<?php

class RequestManage
{
	private $EncryptSeed = "";
	private $EncryptHead = "";
	private $EncryptTail = "";
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

	private function ReqBind($Socket)
	{
		echo "==================== request bind ====================\n";
		// send connect request
		$TotalSize = 100;
		$PDU = array();
		$idx = 0;
		$PDU[$idx++] = pack("S", $TotalSize); // Total_Length UINT2
		$PDU[$idx++] = pack("C", 0x10); // Version UINT1
		$PDU[$idx++] = pack("C", 0x0); // Remain_Packages UINT1
		$PDU[$idx++] = pack("L", $this->CMD_BIND); // Command_ID UINT4
		$PDU[$idx++] = pack("L", 0x1); // Sequence_ID UINT4

		$Gateway_Code = "gw101001";
		$Gateway_Password = "b508ab223783299aed53b1713c5a79b1";
		$MAC = "002421305F5B";
		$PDU[$idx++] = pack("a32", $Gateway_Code); // Gateway_Code String[32]
		$PDU[$idx++] = pack("a32", $Gateway_Password); // Gateway_Password String[32]
		$PDU[$idx++] = pack("a12", $MAC); // MAC String[12]
		$PDU[$idx++] = pack("c", 0x0); // Reconnect_Flag INT1
		$PDU[$idx++] = pack("c", 0x1); // Pad INT1
		$PDU[$idx++] = pack("S", 0x2); // Pad UINT2
		$PDU[$idx++] = pack("L", 101001); // Server_ID UINT4

		$PDU[$idx++] = pack("S", 0x10); // Resv UINT2
		$PDU[$idx++] = pack("S", 0x12); // Checksum UINT2

		$Request = implode("", $PDU);
		socket_write($Socket, $Request, $TotalSize);

		$Buffer = socket_read($Socket, 8);
		$Response = unpack("lResult_Code/LGateway_ID", $Buffer);
		print_r($Response);
	}

	private function ReqUserInfo($Socket)
	{
		echo "==================== request user info ====================\n";
		// send ID information Request
		$TotalSize = 32;
		$PDU = array();
		$idx = 0;
		$PDU[$idx++] = pack("S", $TotalSize); // Total_Length UINT2
		$PDU[$idx++] = pack("C", 0x10); // Version UINT1
		$PDU[$idx++] = pack("C", 0x0); // Remain_Packages UINT1
		$PDU[$idx++] = pack("L", $this->CMD_USER_SERIAL_QUERY); // Command_ID UINT4
		$PDU[$idx++] = pack("L", 0x1); // Sequence_ID UINT4

		$PDU[$idx++] = pack("S", 0x1); // Game_ID UINT2
		$PDU[$idx++] = pack("S", 0x2); // Pad UINT2
		$PDU[$idx++] = pack("L", 0x3); // Gateway_ID UINT4
		$PDU[$idx++] = pack("L", 0x4); // User_ID UINT4
		$PDU[$idx++] = pack("L", 0x5); // Activity_ID UINT4

		$PDU[$idx++] = pack("S", 0x0); // Resv UINT2
		$PDU[$idx++] = pack("S", 0x63); // Checksum UINT2

		$Request = implode("", $PDU);
		socket_write($Socket, $Request, $TotalSize);

		$Buffer = socket_read($Socket, 8);
		$Response = unpack("lResult_Code/LGateway_ID", $Buffer);
		print_r($Response);
	}

	public function DataOperate()
	{
		$Host = "10.10.43.138"; // "192.168.41.89";
		$Port = 5678; // 5091;
		$Proto = getprotobyname("tcp");


		// PDU pack

		// simple client
		$Socket = socket_create(AF_INET, SOCK_STREAM, $Proto) or die("Create socket failed!!");
		$Result = socket_connect($Socket, $Host, $Port) or die("Can not connect to server!!");

		$this->ReqBind($Socket);
		$this->ReqUserInfo($Socket);

		#while ( $Response = socket_read($Socket, 10))
		#{
		#	socket_write($Socket, "End", 3);
		#	echo $Response;
		#}

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
#$resMgr->Analysis();
$reqMgr->DataOperate();

?>


