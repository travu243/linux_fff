cmd_/home/debian/mock_project1/modules.order := {   echo /home/debian/mock_project1/led.ko; :; } | awk '!x[$$0]++' - > /home/debian/mock_project1/modules.order
