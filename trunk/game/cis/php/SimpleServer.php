
<?

class SimpleServer
{
	function __construct()
	{
	}

	private function ResBind($Client)
	{
		echo "==================== response bind ====================\n";
		// bind
		$Buffer = socket_read($Client, 12);
		$Header = unpack("STotal_Length/CVersion/CRemain_Packages/LCommand_ID/LSequence_ID", $Buffer);
		print_r($Header);

		$Buffer = socket_read($Client, 84);
		$Body = unpack("a32Gateway_Code/a32Gateway_Password/a12MAC/cReconnect_Flag/cPad1/SPad2/LServer_ID", $Buffer);
		print_r($Body);

		$Buffer = socket_read($Client, 4);
		$Check = unpack("SResv/SChecksum", $Buffer);
		print_r($Check);

		// 返回码
		$TotalSize = 8;
		$ResList = array();
		$idx = 0;
		$ResList[$idx++] = pack("l", 0x0); // Result_Code INT4
		$ResList[$idx++] = pack("L", 0x12); // Gateway_ID UINT4

		$Response = implode("", $ResList);
		socket_write($Client, $Response, $TotalSize);
	}

	private function ResUserInfo($Client)
	{
		echo "==================== response user info ====================\n";
		$Buffer = socket_read($Client, 12);
		$Header = unpack("STotal_Length/CVersion/CRemain_Packages/LCommand_ID/LSequence_ID", $Buffer);
		print_r($Header);

		$Buffer = socket_read($Client, 16);
		$Body = unpack("SGame_ID/SPad/LGateway_ID/LUser_ID/LActivity_ID", $Buffer);
		print_r($Body);

		$Buffer = socket_read($Client, 4);
		$Check = unpack("SResv/SChecksum", $Buffer);
		print_r($Check);

		// 返回码
		$TotalSize = 8;
		$ResList = array();
		$idx = 0;
		$ResList[$idx++] = pack("l", 0x0); // Result_Code INT4
		$ResList[$idx++] = pack("L", 0x16); // Gateway_ID UINT4

		$Response = implode("", $ResList);
		socket_write($Client, $Response, $TotalSize);

		socket_close($Client); // close client, close after response
	}

	public function Startup()
	{
		$Host = "192.168.43.138";
		$Proto = getprotobyname("tcp");
		set_time_limit(0);
		$Socket = socket_create(AF_INET, SOCK_STREAM, $Proto) or dir("Create socket failed!!\n");
		socket_bind($Socket, $Host, 5091) or die("Can not bind socket!!\n");
		$Result = socket_listen($Socket);

		while ( true )
		{
			$Client = socket_accept($Socket) or die("Can not read input!!\n");
			$this->ResBind($Client);
			#$this->ResUserInfo($Client);
		}

		socket_close($Socket); // close server
	}
}

$Srv = new SimpleServer();
$Srv->Startup();


?>

