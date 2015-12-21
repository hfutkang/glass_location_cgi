#!/bin/bash
killall location.cgi
spawn-fcgi -f /home/kang/glass_location_cgi/location.cgi -p 8081 -a 127.0.0.1 -F 5
