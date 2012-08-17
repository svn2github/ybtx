#!/usr/bin/php
<?php
function line($var){
	$values = explode("|",$var["values"]);
	$labels = explode(",",$var["labels"]);
	$groups = explode(",",$var["groups"]);
	$count  = count($groups);
	$length = count($labels);
	$padding = 10;
	$width = 693;
	$height = 200;
	$text_width = 0;
	
	foreach($groups as $group){
		$arr = ImageTTFBBox(10,0,"simsun.ttc",$group);
		$new_width = $arr[2] - $arr[0];
		if($new_width > $text_width) $text_width = $new_width;
	}
	
	$text_width = $text_width + 30;
	$text_column = floor($width / ($text_width));
	$text_height = 20 * ceil($count / $text_column);
	$img = ImageCreate($width+$padding, $height + $text_height + $padding * 2);
	
	$bg = ImageColorAllocate($img,255,255,255);
	ImageColorTransparent($img,$bg);
	$white = ImageColorAllocate($img,255,255,255);
	$black = ImageColorAllocate($img,0,0,0);
	$gray  = ImageColorAllocate($img,230,230,230);
	
	//数值计算
	$value = explode(",",$values[0]);
	$max = $min = $value[0];
	foreach($values as $value){
		foreach(explode(",",$value) as $v){
			if($v > $max)
				$max = $v;
			else
				if($v < $min)
					$min = $v;
		}
	}
	$log = floor(log(max(abs($min),abs($max)),10));
	$logarr = Imagettfbbox(10,0,"simsun.ttc","(x".pow(10,$log).")");
	$log_text_width = $logarr[2] - $logarr[0];
	$chart_width = $width - $log_text_width - $padding;
	
	$k_max = ceil($max / pow(10,$log));
	$k_min = floor($min / pow(10,$log));
	$k = $k_max - $k_min;
	
	//水平坐标
	$angle = (($chart_width / count($labels))>20)?-30:-90;
	$labels_height = 0;
	foreach($labels as $key => $label){
		$temp_arr = Imagettfbbox(10,$angle,"simsun.ttc",$label);
		$temp_height = $temp_arr[3] - $temp_arr[7];
		if($temp_height > $labels_height) $labels_height = $temp_height;
	}
	$chart_height = $height - $labels_height;
	$labels_width = $chart_width / (count($labels)+1);
	foreach($labels as $key => $label){
		ImageDashedLine($img,$labels_width * $key + $log_text_width + $padding*2,$padding,$labels_width * $key + $log_text_width + $padding*2,$chart_height+$padding,$gray);
		ImageTTFText($img, 8, $angle, $labels_width * $key + $log_text_width + $padding*2, $chart_height + $padding + 10, $black, "simsun.ttc", $label);
	}
	
	//垂直坐标
	$a = $chart_height / ($k * pow(10, $log));
	$k_height = $chart_height / $k;
	if($k>10) 			$add = 2;
	else if($k>4) 	$add = 1;
	else if($k>2) 	$add = 0.5;
	else if($k==1) 	$add = 0.25;
	for($i = 0; $i < $k; $i=$i+$add){
		ImageLine($img,$log_text_width + $padding, $k_height * $i + $padding, $width, $k_height * $i + $padding, $gray);
		$arr = ImageTTFBBox(10,0,"simsun.ttc",($k_max-$i)*pow(10,$log));
		$left = $log_text_width - $arr[2] + $arr[0];
		ImageTTFText($img,10,0,$left,$k_height * $i+$padding+10,$black,"simsun.ttc",($k_max-$i)*pow(10,$log));
	}
	
	//坐标轴
	ImageLine($img, $log_text_width + $padding, $padding, $log_text_width + $padding, $chart_height+$padding, $black);
	ImageLine($img, $log_text_width + $padding, $chart_height+$padding, $width, $chart_height+$padding, $black);
	
	//画线，线备注
	$r = $c = 0;
	foreach($groups as $key => $group){
		getColor($key / $count, $c_r, $c_b, $c_g);
		$c1 = ImageColorAllocate($img, $c_r*0.7,$c_g*0.7,$c_b*0.7);
		$c2 = ImageColorAllocate($img, $c_r,$c_g,$c_b);
		$value_arr = explode(",",$values[$key]);
		foreach($value_arr as $index => $value){
			if($index < count($value_arr)-1){
				ImageLine($img,$labels_width * ($index) + $log_text_width + $padding*2,($k_height * $k_max + $padding) - $value * $a,$labels_width * ($index+1) + $log_text_width + $padding*2,($k_height * $k_max + $padding) - $value_arr[$index+1] * $a,$c2);
				ImageLine($img,$labels_width * ($index) + $log_text_width + $padding*2,($k_height * $k_max + $padding) - $value * $a,$labels_width * ($index+1) + $log_text_width + $padding*2,($k_height * $k_max + $padding) - $value_arr[$index+1] * $a,$c2);
			}
			diamond($img,$labels_width * ($index) + $log_text_width + $padding*2,($k_height * $k_max + $padding) - $value * $a,$c1,$c2);
		}
		
		diamond($img, $log_text_width + $padding*2 + $text_width * $c, $height + $padding*3 + ($r * 20), $c1, $c2);
		ImageTTFText($img, 10, 0, $log_text_width + $padding*2 + $text_width * $c + 10, $height + $padding*3 + ($r * 20) + 5, $black, "simsun.ttc", $group);
		if(++$c == $text_column){
			$r++;
			$c = 0;
		}
	}
	
	ImagePNG($img);
	ImageDestroy($img);
}

function diamond($img,$x,$y,$c1,$c2){
	$r = 5;
	ImageLine($img,$x-$r,$y,$x,$y-$r,$c1);
	ImageLine($img,$x,$y-$r,$x+$r,$y,$c1);
	ImageLine($img,$x+$r,$y,$x,$y+$r,$c1);
	ImageLine($img,$x,$y+$r,$x-$r,$y,$c1);
	ImageFillToBorder($img,$x,$y,$c1,$c2);
}

function bar($var){
	$values = explode(",",$var["values"]);
	$labels = explode(",",$var["labels"]);
	$count = count($values);
	$padding = 10;
	$height = $count*20+20;
	$width = 703;
	
	$max = $values[0];
	$min = $values[0];
	$text_width = 0;
	$text_width_array = array();
	while(list($key,$item) = each($values)){
		$max = max($max,$item);
		$min = min($min,$item);
		$text_box = ImageTTFBBox(10,0,"simsun.ttc",$labels[$key]);
		$text_width_value = $text_box[2]-$text_box[0];
		array_push($text_width_array,$text_width_value);
		$text_width = max($text_width, $text_width_value);
	}
	$max_width = $width - $text_width - $padding*2;
	if($max<0) $max=0;
	if($min>0) $min=0;
	$m_l = floor(log(abs($min),10));		//数量级
	$m_r = floor(log($max,10));
	$m = max($m_l,$m_r);
	$k_l = ceil(abs($min)/pow(10,$m));	//倍数
	$k_r = ceil($max/pow(10,$m));
	$k = $k_l + $k_r;
	$a = $max_width / ($k*pow(10,$m));	//系数
	$o = $k_l * pow(10,$m) * $a;				//原点位置
	
	//图片初始化
	$img = imagecreate($max_width+$text_width+$padding*2,$height+$padding*2);
	$bg = ImageColorAllocate($img,255,255,255);
	ImageColorTransparent($img,$bg);
	$white = ImageColorAllocate($img,255,255,255);
	$black = ImageColorAllocate($img,0,0,0);
	$gray  = ImageColorAllocate($img,230,230,230);
	
	//绘制正文信息
	reset($values);
	while(list($i,$item) = each($values)){
		getColor(($i+1)/$count, $r, $g, $b);
		ImageLine($img,0,$i*20+$padding+17,$max_width+$text_width+$padding*2,$i*20+$padding+17,$gray);
		ImageFilledRectangle($img, $o, $i*20+$padding+19, $o+$text_width+$padding*2, $i*20+$padding+35, ImageColorAllocate($img,$r*0.7,$g*0.7,$b*0.7));
		ImageFilledRectangle($img, $o, $i*20+$padding+20, $o+$text_width+$padding*2, $i*20+$padding+34, ImageColorAllocate($img,$r,$g,$b));
		if($item<0){
			$x = $o+$item*$a;
			ImageFilledRectangle($img, $o, $i*20+$padding+19, $x, $i*20+$padding+35, ImageColorAllocate($img,$r*0.7,$g*0.7,$b*0.7));
			ImageFilledRectangle($img, $o, $i*20+$padding+20, $x+1,  $i*20+$padding+34, ImageColorAllocate($img,$r,$g,$b));
		}else{
			$x = $o+$item*$a;
			ImageFilledRectangle($img, $o+$text_width+$padding*2, $i*20+$padding+19, $x+$text_width+$padding*2, $i*20+$padding+35, ImageColorAllocate($img,$r*0.7,$g*0.7,$b*0.7));
			ImageFilledRectangle($img, $o+$text_width+$padding*2, $i*20+$padding+20, $x+$text_width+$padding*2-1,  $i*20+$padding+34, ImageColorAllocate($img,$r,$g,$b));
			
		}
		$offset = ($text_width - $text_width_array[$i]) / 2;
		ImageTtfText($img,10,0,$o+$padding+$offset,$i*20+$padding+32,$black,"simsun.ttc",$labels[$i]);
	}
	
	//绘制参考线
	ImageTtfText($img,10,0,$o+2+$text_width+$padding*2,$padding+10,$black,"cour.ttf","0");
	ImageLine($img, $o, $padding, $o, $padding+$height, $black);
	ImageLine($img, $o+$text_width+$padding*2, $padding, $o+$text_width+$padding*2, $padding+$height, $black);
	ImageTtfText($img,10,0,$o,$padding+10,$black,"cour.ttf","(0x".pow(10,$m).")");
	if($k>10){
		for($i = 1; $i < $k_l+1; $i++){
			if($i%2==0 && $i!=$k_l){
				$x = $i*$a*pow(10,$m);
				ImageTtfText($img,10,0,$o-$x+2,$padding+10,$black,"cour.ttf","-".$i*pow(10,$m));
				ImageLine($img, $o-$x, $padding, $o-$x, $padding+$height, $black);
			}
		}
		for($i = 1; $i < $k_r+1; $i++){
			if($i%2==0 & $i!=$k_r){
				$x = $i*$a*pow(10,$m);
				ImageTtfText($img,10,0,$o+$x+2+$text_width+$padding*2,$padding+10,$black,"cour.ttf",$i*pow(10,$m));
				ImageLine($img, $o+$x+$text_width+$padding*2, $padding, $o+$x+$text_width+$padding*2, $padding+$height, $black);
			}
		}
	}else if($k>4){
		for($i = 1; $i < $k_l+1; $i++){
			if($i!=$k_l){
				$x = $i*$a*pow(10,$m);
				ImageTtfText($img,10,0,$o-$x+2,$padding+10,$black,"cour.ttf","-".$i);
				ImageLine($img, $o-$x, $padding, $o-$x, $padding+$height, $black);
			}
		}
		for($i = 1; $i < $k_r+1; $i++){
			if($i!=$k_r){
				$x = $i*$a*pow(10,$m);
				ImageTtfText($img,10,0,$o+$x+2+$text_width+$padding*2,$padding+10,$black,"cour.ttf",$i);
				ImageLine($img, $o+$x+$text_width+$padding*2, $padding, $o+$x+$text_width+$padding*2, $padding+$height, $black);
			}
		}
	}else if($k>2){
		for($i = 1; $i < $k_l+1; $i++){
			$x = ($i-0.5)*$a*pow(10,$m);
			ImageTtfText($img,10,0,$o-$x+2,$padding+10, $black, "cour.ttf","-".($i-0.5)*pow(10,$m));
			ImageDashedLine($img, $o-$x, $padding, $o-$x, $padding+$height, $black);
			if($i!=$k_l){
				$x = $i*$a*pow(10,$m);
				ImageTtfText($img,10,0,$o-$x+2,$padding+10,$black,"cour.ttf","-".$i*pow(10,$m));
				ImageLine($img, $o-$x, $padding, $o-$x, $padding+$height, $black);
			}
		}
		for($i = 1; $i < $k_r+1; $i++){
			$x = ($i-0.5)*$a*pow(10,$m);
			ImageTtfText($img,10,0,$o+$x+2+$text_width+$padding*2,$padding+10, $black, "cour.ttf",($i-0.5)*pow(10,$m));
			ImageDashedLine($img, $o+$x+$text_width+$padding*2, $padding, $o+$x+$text_width+$padding*2, $padding+$height, $black);
			if($i!=$k_r){
				$x = $i*$a*pow(10,$m);
				ImageTtfText($img,10,0,$o+$x+2+$text_width+$padding*2,$padding+10,$black,"cour.ttf",$i*pow(10,$m));
				ImageLine($img, $o+$x+$text_width+$padding*2, $padding, $o+$x+$text_width+$padding*2, $padding+$height, $black);
			}
		}
	}else{
		for($i = 1; $i < $k_l+1; $i++){
			$x = ($i-0.5)*$a*pow(10,$m);
			ImageTtfText($img,10,0,$o-$x+2,$padding+10, $black, "cour.ttf","-".($i-0.5)*pow(10,$m));
			ImageLine($img, $o-$x, $padding, $o-$x, $padding+$height, $black);
			$x = ($i-0.25)*$a*pow(10,$m);
			ImageTtfText($img,10,0,$o-$x+2,$padding+10, $black, "cour.ttf","-".($i-0.25)*pow(10,$m));
			ImageDashedLine($img, $o-$x, $padding, $o-$x, $padding+$height, $black);
			$x = ($i-0.75)*$a*pow(10,$m);
			ImageTtfText($img,10,0,$o-$x+2,$padding+10, $black, "cour.ttf","-".($i-0.75)*pow(10,$m));
			ImageDashedLine($img, $o-$x, $padding, $o-$x, $padding+$height, $black);
		}
		for($i = 1; $i < $k_r+1; $i++){
			$x = ($i-0.5)*$a*pow(10,$m);
			ImageTtfText($img,10,0,$o+$x+2+$text_width+$padding*2,$padding+10, $black, "cour.ttf",($i-0.5)*pow(10,$m));
			ImageLine($img, $o+$x+$text_width+$padding*2, $padding, $o+$x+$text_width+$padding*2, $padding+$height, $black);
			$x = ($i-0.25)*$a*pow(10,$m);
			ImageTtfText($img,10,0,$o+$x+2+$text_width+$padding*2,$padding+10, $black, "cour.ttf",($i-0.25)*pow(10,$m));
			ImageDashedLine($img, $o+$x+$text_width+$padding*2, $padding, $o+$x+$text_width+$padding*2, $padding+$height, $black);
			$x = ($i-0.75)*$a*pow(10,$m);
			ImageTtfText($img,10,0,$o+$x+2+$text_width+$padding*2,$padding+10, $black, "cour.ttf",($i-0.75)*pow(10,$m));
			ImageDashedLine($img, $o+$x+$text_width+$padding*2, $padding, $o+$x+$text_width+$padding*2, $padding+$height, $black);
		}
	}
	ImagePNG($img);
	ImageDestroy($img);
}
function pie($var){
	$val = explode(",",$var["values"]);
	$desc = explode(",",$var["labels"]);
	$sum = array_sum($val);
	$count = count($val);
	$width = 200;
	$padding = 15;
	$max_width = 400;
	
	$height = $width * 0.5;
	$text_height = $count*20;
	if($height < $text_height){
		$height = $text_height;
		if($height > $max_width){
			$center_y = $height / 2 + $padding;
			$height = $max_width;
		}
		$width = $height*(1+(1-($height-($width/2))/($max_width-($width/2))));
	}else{
		$text_height = $height;
	}
	$think = (($width-$height)*30)/($width);
	$center_x = $width/2+$padding+0.5;
	$center_y = $text_height/2+$padding+0.5;
	
	$img = imagecreate($width+$padding*2+300,$text_height+$think+$padding*2);
	$bg = ImageColorAllocate($img,255,255,255);
	ImageColorTransparent($img,$bg);
	$white = ImageColorAllocate($img,255,255,255);
	$black = ImageColorAllocate($img,0,0,0);
	$blue  = ImageColorAllocate($img,0,255,0);
	ImageArc($img,$center_x,$center_y,$width,$height,180,360,$white);
	ImageArc($img,$center_x,$center_y+$think,$width,$height,0,180,$white);
	ImageLine($img,$center_x-$width/2,$center_y,$center_x-$width/2,$center_y+$think,$white);
	ImageLine($img,$center_x+$width/2,$center_y,$center_x+$width/2,$center_y+$think,$white);
	
	$i = 0;
	$c = 0;
	ImageLine($img,$center_x,$center_y,$center_x+$width/2,$center_y,$black);
	$p_c = array();
	$p_x = array();
	$p_y = array();
	$p_a = array();
	foreach($val as $v){
		$p = $v/$sum;
		$t = $p*pi() + $c;
		$c += $p*pi()*2;
		$y1 = sin($c)*$height/2+$center_y;
		$x1 = cos($c)*$width/2+$center_x;
		
		$r = 255;
		$g = 80;
		$b = 80;
		
		$a = ++$i/$count;
		getColor($a, $r, $g, $b);
		
		$x = cos($t)*$width/3.5+$center_x;
		$y = sin($t)*$height/3.5+$center_y;
		
		$item = array();
		$item["r"]	= $r;
		$item["g"]	= $g;
		$item["b"]	= $b;
		$item["x"]	= $x1;
		$item["y"]	= $y1;
		array_push($p_c,$item);
		array_push($p_x,$x);
		array_push($p_y,$y);
		array_push($p_a,round($p*100,2));
		if($p>0.01){
			$color = ImageColorAllocate($img,round($r*0.7),round($g*0.7),round($b*0.7));
			//if($aaaaa<10)
			ImageFill($img,$x,$y,$white);
			//if($aaaaa<10)
			ImageFill($img,$x,$y,$color);
			if($i!=$count){
				ImageSetPixel($img,$center_x+1,$center_y,$white);
				ImageLine($img,$center_x,$center_y,$x1,$y1,$white);
				if($y1>$center_y){
					ImageLine($img,$x1,$y1,$x1,$y1+$think-1,$white);
				}
			}else{
				ImageLine($img,$center_x,$center_y,$x1,$y1,$color);
			}
		}
	}
	ImageArc($img,$center_x,$center_y,$width,$height,180,360,$white);
	ImageArc($img,$center_x,$center_y,$width,$height,0,180,$white);
	ImageFillToBorder($img,$center_x,$center_y,$white,$white);
	ImageLine($img,$center_x,$center_y,$center_x+$width/2,$center_y,$black);
	foreach($p_c as $key => $item){
		$x = $p_x[$key];
		$y = $p_y[$key];
		if($p_a[$key]>1){
			$color = ImageColorAllocate($img,$item['r'],$item['g'],$item['b']);
			ImageFill($img,$x,$y,$white);
			ImageFill($img,$x,$y,$color);
			if($key!=$count-1){
				ImageSetPixel($img,$center_x+1,$center_y,$white);
				ImageLine($img,$center_x,$center_y,$item['x'],$item['y'],$white);
			}else{
				ImageLine($img,$center_x,$center_y,$item['x'],$item['y'],$color);
			}
		}
	}
	array_multisort($p_y,$p_x,$p_c,$p_a,$val,$desc);
	$i = 0;
	foreach($p_y as $k => $y){
		$x = $p_x[$k];
		$y2 = ($i++)*20+$padding+$think/2;
		$x2 = abs($y-$y2)+$x;
		$x3 = $center_x+$width/2+$padding;
		$a = $p_a[$k];
		if($a<10) $a = " ".$a;
		if(strlen($a)<3) $a.=".";
		if(strlen($a)<4) $a.="0";
		if(strlen($a)<5) $a.="0";
		$a = $a."%";
		ImageLine($img, $x,$y,$x2,$y2,$black);
		ImageLine($img, $x2,$y2,$x3,$y2,$black);
		ImageTtfText($img,10,0,$x3+$padding,$y2+5,$black,"cour.ttf",$a);
		ImageTtfText($img,10,0,$x3+$padding+60,$y2+5,$black,"simsun.ttc",$desc[$k]);
	}
	ImagePNG($img);
	ImageDestroy($img);
}

function Getcolor($a, &$r, &$g, &$b){
	$a = 6 * $a;
	if($a<1){
		$r = 255;
		$g = 80;
		$b = round((175*($a-0))+80);
	}else if($a<2){
		$r = round(255-(175*($a-1)));
		$g = 80;
		$b = 255;
	}else if($a<3){
		$r = 80;
		$g = round((175*($a-2))+80);
		$b = 255;
	}else if($a<4){
		$r = 80;
		$g = 255;
		$b = round(255-(175*($a-3)));
	}else if($a<5){
		$r = round((175*($a-4))+80);
		$g = 255;
		$b = 80;
	}else if($a<6){
		$r = 255;
		$g = round(255-(175*($a-5)));
		$b = 80;
	}
}

header("Content-type: image/jpeg;filename=image.png");
$var = $_REQUEST;
$var["flag"]($var);
?>