
<?php
//phpinfo();
//echo $_GET;
//echo $_POSt;

class LogManage
{
	private $FileName;
	private $Content;

	function __construct()
	{
		foreach ($_GET as $name => $value)
		{
			error_log( "$name => $value\n");
		}

		foreach ($_POST as $name => $value)
		{
			error_log( "$name => $value\n");
		}
	} 

	public function GetContent()
	{
		$buffer = file_get_contents('php://input');
		list($this->FileName, $this->Content) = explode("=", $buffer);
		#error_log("$this->FileName => $this->Content");
		#error_log(getenv('HTTP_X_FORWARDED_FOR'));
	}

	public function ExportContent()
	{
		if ( isset($_SERVER['HTTP_X_FORWARDED_FOR']) )
			$IPAddress = getenv('HTTP_X_FORWARDED_FOR');
		else
			$IPAddress = getenv('REMOTE_ADDR');
		$HeadFlag = "gac_err_" . $IPAddress . "_";
		$FilePath = "./upload/" . preg_replace("/^gac_err/i", $HeadFlag, $this->FileName);

		if ( file_exists($FilePath) )
			return;

		$fp = fopen($FilePath, 'w');
		fwrite($fp, $this->Content);
		fclose($fp);
	}
}

$logMgr = new LogManage();
$logMgr->GetContent();
$logMgr->ExportContent();

?>

