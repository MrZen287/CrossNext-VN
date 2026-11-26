
param([Parameter(Mandatory)][string[]]$Sheets,[Parameter(Mandatory)][string]$Output,[Parameter(Mandatory)][string]$Pairs)
Add-Type -AssemblyName System.Drawing
$ErrorActionPreference='Stop'
function Write-Mask($out,$name,$w,$h,[byte[]]$ink,[byte[]]$opaque){
 foreach($pair in @(@('Ink',$ink),@('Opaque',$opaque))){
  $out.WriteLine("inline constexpr uint8_t $name$($pair[0])[] = {")
  $bytes=$pair[1]
  for($i=0;$i -lt $bytes.Length;$i+=40){
   $last=[Math]::Min($i+39,$bytes.Length-1)
   $out.WriteLine((($bytes[$i..$last] | ForEach-Object { [string]$_ }) -join ',')+',')
  }
  $out.WriteLine('};')
 }
 $out.WriteLine("inline constexpr zenart::Mask ${name}{$w,$h,${name}Ink,${name}Opaque};")
}
function Build-Mask($out,$name,$w,$h,$sample){
 $solid=[bool[]]::new($w*$h);$outside=[bool[]]::new($w*$h)
 for($y=0;$y -lt $h;$y++){for($x=0;$x -lt $w;$x++){ $solid[$y*$w+$x]= & $sample $x $y }}
 $todo=[Collections.Generic.Queue[int]]::new()
 for($y=0;$y -lt $h;$y++){for($x=0;$x -lt $w;$x++){
  $at=$y*$w+$x
  if(($x -eq 0 -or $x -eq $w-1 -or $y -eq 0 -or $y -eq $h-1) -and !$solid[$at]){
   $outside[$at]=$true;$todo.Enqueue($at)
  }
 }}
 while($todo.Count){
  $at=$todo.Dequeue();$x=$at%$w
  foreach($next in @(($at-$w),($at+$w),$(if($x -gt 0){$at-1}else{-1}),$(if($x+1 -lt $w){$at+1}else{-1}))){
   if($next -ge 0 -and $next -lt $solid.Length -and !$solid[$next] -and !$outside[$next]){
    $outside[$next]=$true;$todo.Enqueue($next)
   }
  }
 }
 $stride=[int][Math]::Ceiling($w/8.0)
 $ink=[byte[]]::new($stride*$h);$opaque=[byte[]]::new($stride*$h)
 for($y=0;$y -lt $h;$y++){for($x=0;$x -lt $w;$x++){
  $at=$y*$w+$x;$byteAt=$y*$stride+($x -shr 3);$bit=128 -shr ($x%8)
  if($solid[$at]){$ink[$byteAt]=$ink[$byteAt] -bor $bit}
  if(!$outside[$at]){$opaque[$byteAt]=$opaque[$byteAt] -bor $bit}
 }}
 Write-Mask $out $name $w $h $ink $opaque
}
# Source coordinates measured against the approved illustration boards.
$back=@(178,589,1025,1477,155,543,999,1453,230,751,1274,1830)
$front=@(214,627,1062,1510,191,583,1039,1492,273,785,1316,1866)
$right=@(419,856,1290,1744,424,843,1288,1745,476,992,1530,2087)
$edge=@(254,255,254,254,256,255,255,255,225,225,225,225)
$end=@(567,567,567,567,594,593,594,594,475,475,475,475)
$smallX=@(123,546,978,1414,27,483,894,1351,169,681,1194,1727)
$smallEnd=@(278,726,1150,1570,162,591,1024,1492,317,809,1353,1878)
$smallY=@(615,615,615,615,677,677,677,677,504,504,504,504)
$smallBottom=@(718,718,718,718,779,779,779,779,594,594,594,594)
$out=[IO.StreamWriter]::new($Output,$false,[Text.UTF8Encoding]::new($false))
try{
 $out.WriteLine('// Generated from the approved zodiac concept boards.')
 $images=@($Sheets | ForEach-Object { [Drawing.Bitmap]::new($_) })
 $pairsImage=[Drawing.Bitmap]::new($Pairs)
 for($n=0;$n -lt 12;$n++){
  $im=$images[$n -shr 2];$tileWidth=$im.Width/4.0;$tileStart=[int]($tileWidth*($n%4))
  Build-Mask $out "Main$n" 144 260 {
   param($x,$y)
   $px=2*$x;$py=2*$y
   $coverY=193-$px*0.06
   $overLeft= if($n -eq 5){$px -lt 116 -or ($py -gt 402 -and $px -lt 195)}elseif($n -eq 4){$px -lt 120 -or ($py -gt 416 -and $px -gt 153)}else{$px -lt 61 -or ($py -ge 238 -and $py -le 363 -and $px -lt 117)}
   $overTop= $n -ne 5 -and $px -gt 145 -and $py -lt $coverY+37
   if($py -ge $coverY-5 -and !$overLeft -and !$overTop){return $false}
   $sx=if($px -lt 61){$tileStart+3+($back[$n]-$tileStart-3)*$px/61.0}
       elseif($px -lt 77){$back[$n]+($front[$n]-$back[$n])*($px-61)/16.0}
       else{$front[$n]+($right[$n]-$front[$n])*($px-77)/200.0}
   $sy=if($py -lt $coverY){$edge[$n]*$py/$coverY}else{$edge[$n]+($end[$n]-$edge[$n])*($py-$coverY)/(530-$coverY)}
   if($sx -lt $tileStart -or $sx -ge [Math]::Min($im.Width-1,$tileStart+$tileWidth) -or $sy -ge $im.Height){return $false}
   return $im.GetPixel([int]$sx,[int]$sy).R -lt 175
  }
  Build-Mask $out "Cloud$n" 112 63 {
   param($x,$y)
   $tileX=($n%4)*362
   $sy0=@(35,372,730)[$n -shr 2]
   $sy1=@(206,568,914)[$n -shr 2]
   $sx=$tileX+18+318*$x/111.0
   $sy=$sy0+($sy1-$sy0)*$y/62.0
   return $pairsImage.GetPixel([int]$sx,[int]$sy).R -lt 175
  }
 }
 $out.WriteLine('inline constexpr const zenart::Mask* mainArt[] = {'+ ((0..11 | ForEach-Object {"&Main$_"}) -join ', ') + '};')
 $out.WriteLine('inline constexpr const zenart::Mask* cloudArt[] = {'+ ((0..11 | ForEach-Object {"&Cloud$_"}) -join ', ') + '};')
}finally{
 $out.Dispose()
 foreach($im in $images){$im.Dispose()}
 if($pairsImage){$pairsImage.Dispose()}
}
