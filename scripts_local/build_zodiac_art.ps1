
param([Parameter(Mandatory)][string[]]$Sheets,[Parameter(Mandatory)][string]$Output)
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
$back=@(149,548,1010,1458,112,538,994,1429,218,720,1262,1790)
$front=@(181,584,1036,1488,153,580,1024,1465,243,745,1289,1820)
$right=@(417,852,1287,1717,414,843,1290,1739,477,975,1505,2037)
$edge=@(205,207,209,210,230,234,236,236,183,186,187,184)
$end=@(568,568,568,568,623,623,625,625,472,472,472,472)
$smallX=@(123,546,978,1414,27,483,894,1351,169,681,1194,1727)
$smallEnd=@(278,726,1150,1570,162,591,1024,1492,317,809,1353,1878)
$smallY=@(615,615,615,615,677,677,677,677,504,504,504,504)
$smallBottom=@(718,718,718,718,779,779,779,779,594,594,594,594)
$out=[IO.StreamWriter]::new($Output,$false,[Text.UTF8Encoding]::new($false))
try{
 $out.WriteLine('// Generated from the approved zodiac concept boards.')
 $images=@($Sheets | ForEach-Object { [Drawing.Bitmap]::new($_) })
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
  Build-Mask $out "Cloud$n" 72 53 {
   param($x,$y)
   $sx=$smallX[$n]+($smallEnd[$n]-$smallX[$n])*$x/71.0
   $sy=$smallY[$n]+($smallBottom[$n]-$smallY[$n])*$y/52.0
   return $im.GetPixel([int]$sx,[int]$sy).R -lt 168
  }
 }
 $out.WriteLine('inline constexpr const zenart::Mask* mainArt[] = {'+ ((0..11 | ForEach-Object {"&Main$_"}) -join ', ') + '};')
 $out.WriteLine('inline constexpr const zenart::Mask* cloudArt[] = {'+ ((0..11 | ForEach-Object {"&Cloud$_"}) -join ', ') + '};')
}finally{
 $out.Dispose()
 foreach($im in $images){$im.Dispose()}
}
