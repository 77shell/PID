

while($line = <>)
{
    if($line =~ m/.*archive/) { }
    elsif($line =~ m/.*cppunit/) { }
    elsif($line =~ m/.*acu-stm32-para/) { }
    elsif($line =~ m/.*acu-stm32-lcd/) { }
    elsif($line =~ m/.*stlib.*/) { }
    else { print " $line"; }
}
