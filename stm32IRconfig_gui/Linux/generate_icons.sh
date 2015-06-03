ICONS='Icon.gif MiniIcon.ico'
### reswrap version 4:
#reswrap -i -e -o icons.h $ICONS
#reswrap -o icons.cpp $ICONS
### include icons.h in icons.cpp is missing
### not yet there in version 4

### reswrap version 5.2:
reswrap -h -o icons.h $ICONS -s -i icons.h -o icons.cpp $ICONS
