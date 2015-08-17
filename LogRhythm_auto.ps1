####################################################################################################
# LogRhythm_auto.ps1
# Paul Ackerman
# V 1.0	August 2015
#
# This script is one component of several referenced by the paper:
# "The LogLED - An LED-Based Information Security Dashboard" by Paul Ackerman available
# at 
# 
# The purpose of this script is to provide a capability for the InfoSec
# team to monitor critical events and the defence condition of the network. 
# The script connects to the LogRhythm API to obtain information about current
# alarms and sends that information to a Teensy micro-controller that drives
# an LPD8806 LED strip.
#
# The script is heavily commented. Please refer to the comments for
# specific functionality.
#
# If you make improvements to the script, please share them with:
# PAckerman@VacciNetLLC.com
#
####################################################################################################

$com = $null  
 
If ($Host.Name -match 'ise') 
{ 
    Write-Host "WARNING: This script must run in the console`n`Right click script and select 'Run with PowerShell'" 
    break; 
} 
 
 # change VId/PId string to match your device (if it's not a standard Teensy) 
# also following code assumes only one Teensy is connected 
 
try 
{ 
    gwmi Win32_SerialPort -EA 1|  
    where {$_.PNPDeviceID-match"16C0&PID_0487" -or $_.PNPDeviceID-match"16C0&PID_0483"}|  
    foreach{$com = $_.DeviceID; $com} 
} 
catch{$_}  
 
If ($com -eq $null) 
{ 
    "Can't find Teensy, check it is connected and that you have not changed VId/PId" 
    break 
} 
 
try 
{ 
    $port=new-Object System.IO.Ports.SerialPort $com,9600,None,8,one 
    $port.ReadTimeout = 90  
    $port.WriteTimeout = 90 
} 
catch{}    

# Main loop
while($true)
{     
		# $APIError is used to keep track of when an error occurs reaching the LogRhythm API
		# When $APIError is false, no error has occurred.
		$APIerror = $false
		
		# This script assumes the LogRhythm web service has been configured with
		# Windows Authentication and this script is being executed by a user with permission
		# to access the API.
		[System.Net.ServicePointManager]::ServerCertificateValidationCallback = {$true}
		# Create the URI to be called
		$URI = "https://logmanagerhostname:4443/LogRhythm.API/Services/AlarmServiceWindowsAuth.svc?wsdl"

		try
		{
			# Call the URI using windows auth
			$AlarmService = New-WebServiceProxy -uri $URI -UseDefaultCredential 
		}
		catch
		{
			# There was an error connecting to the API.
			$APIerror = $true
		}

		# Set Parameters
		$startdate = Get-Date
		$startdate = $startdate.AddDays(-90)
		$enddate = Get-Date
		$enddate = $endDate.AddDays(1)

		try
		{
			# Pull the alarms from the last 90 days that have a status of "New". $Results will be an array of alarms including
			# various information such alarm creation date, status and severity
			$Results = $AlarmService.GetFirstPageAlarmsByAlarmStatus($startdate, $true,$enddate, $true,"New", $true,$true, $true,5000,$true)
		}
		catch
		{
			$APIerror = $true
		}

		# The number of alarms will be tracked in order to display an effect when the number of alarms increases
		$numAlarms = $Results.Alarms.Count

		# This dumps just the severity rating from the alarms into an array called $RBP. This is a numeric score from 0 to 100
		# and will be used to determine the color value for the alarm
		$RBP = $Results.Alarms.RBPAvg

		# These variable represent the quantity of LEDs that will be illuminated of each color. They must all start at 0.
		$green = 0
		$blue = 0
		$orange = 0
		$red = 0

		# Here we loop through all the alarms and increment the $green, $blue, $orange and $red values depending on the severity of the alarm.
		# Feel free to adjust the risk scores below to suit your liking.
		foreach ($code in $RBP)
		{
		if ($code -lt 60)
		{
			# Alarms with scores below 60 will be green.
			$green = $green + 1
		}
		elseif($code -le 69)
		{
			# Alarms with scores between 60 and 69 will be blue. This started as yellow but yellow and orange can be difficult for
			# some people to differentiate so blue was chosen to replace yellow.
			$blue = $blue + 1
		}
		elseif($code -le 79)
		{
			# Alarms with scores between 70 and 79 will be orange.
			$orange = $orange + 1
		}
		else
		{
			# Alarms with scores 80 or above will be red.
			$red = $red + 1
		}
		}

		# Write the number of each color and the number of alarms to the console for informational purposes
		write-host "Green:" $green " Blue:" $blue " Orange:" $orange " Red:" $red
		$numAlarms
		$numAlarms = $numAlarms - 1

		# The Teensy looks for serial data beginning with an "@". All other data is considered garbage
		$LEDcode = "@"
		
		# The effect variable is used to represent which effect to display on the teensy. This is the 98th argument
		$effect = 0
		
		# LEDCount is used to keep track of how many LED's have been addressed so far. We run into a problem when
		# there are more alarms than LED's so we can only address the first 32 alarms. If more than 32 alarms exist
		# their values will be discarded and the top LED will be made to blink indicating that more alarms exist.
		$LEDcount = 0
		
		# Build LED string to send to the teensy
		# The Teensy expects a red, green, blue, value for each of the 32 LED'score, a blink value and an effect
		# ending in a comma. Example:
		# "@r,g,b,r,g,b,...,r,g,b,blink,effect,"
		# Acceptable values for red,green,blue parameters are 0-127.
		# Acceptable values for the blink parameter are 0-32. If 32 is used, no LED will blink as they are addressed 0-31
		# Acceptable values for the effect parameter are 0-9. Refer to the Teensy sketch documentation to determine
		# what each effect code will do as this may change independently of this PowerShell script.
		
		if ($numAlarms -eq -1)
		{
			# There are no alarms. Great job! Your reward is a random effect followed by turning the strip off.
			$effect = Get-Random -minimum 2 -maximum 9
			$LEDcode = "@0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0," + $effect + ","
		}
		else
		{
		# Here, we're adding the string "0,32,0," which is a dim green LED however many times we incremented the "$green"
		# counter earlier when we looped through all the alarms $RBP.
		for ($i=0; $i -lt $green; $i++)
		{
			if($LEDcount -lt 32)
			{
				$LEDcode = $LEDcode + "0,32,0,"
				$LEDcount++
			}
		}
		# Same as above for blue, orange and red
		for ($i=0; $i -lt $blue; $i++)
		{
			if($LEDcount -lt 32)
			{
				$LEDcode = $LEDcode + "0,0,64,"
				$LEDcount++
			}
		}
		for ($i=0; $i -lt $orange; $i++)
		{
			if($LEDcount -lt 32)
			{
				$LEDcode = $LEDcode + "32,8,0,"
				$LEDcount++
			}
		}
		for ($i=0; $i -lt $red; $i++)
		{
			if($LEDcount -lt 32)
			{
				$LEDcode = $LEDcode + "32,0,0,"
				$LEDcount++
			}
		}
		# Fill in the remaining LED's as off.
		for ($i=0; $i -lt (31-$numAlarms); $i++)
		{
			if($LEDcount -lt 32)
			{
				$LEDcode = $LEDcode + "0,0,0,"
				$LEDcount++
			}
		}
	
		# Here we're saying if the number of alarms has increased, display effect 2 and then show the alarms. Otherwise, just show the alarms
		if ($numAlarms -gt $prevalarms)
		{
			$effect = ",2,"
		}
		else
		{
			$effect = ",0,"
		}
		
		$blink = $numAlarms
		$prevalarms = $numAlarms
		
		# If we have more alarms than will fit on the LED strip, blink the top LED to indicate this.
		if ($blink -ge 31)
		{
			$blink = 31
		}
		# Otherwise, blink LED 32 which doesn't exist - ie don't blink any of them.
		else
		{
			$blink = 32
		}
		
		# Concantenate the LED values, blink and effect strings in preparation to send to the teensy
		$LEDcode = $LEDcode + $blink + $effect

		}
		
		# If there was an error talking to the LogRhythm API, turn the whole strip purple.
		if ($APIerror)
		{
			$LEDcode = "@40,0,40,40,0,40,40,0,40,40,0,40,40,0,40,40,0,40,40,0,40,40,0,40,40,0,40,40,0,40,40,0,40,40,0,40,40,0,40,40,0,40,40,0,40,40,0,40,40,0,40,40,0,40,40,0,40,40,0,40,40,0,40,40,0,40,40,0,40,40,0,40,40,0,40,40,0,40,40,0,40,40,0,40,40,0,40,40,0,40,40,0,40,40,0,40,0,0,"
		}
		
		# Write the string to the serial port (send to Teensy)
		if ($LEDcode)
		{ 
		$LEDcode
			try 
			{ 
				$port.open()
				$port.Write($LEDcode) 
				$port.close()
			} 
			catch [TimeoutException] 
			{} 
		} 
				
		# For SOC's that aren't 24-7, uncomment the code below to turn the strip off from 5:00pm to 7:30am
		$stoptime = get-date "5:00:00 pm"
		$stoptimeend = $stoptime.AddMinutes(1)
		
		if ((get-date) -ge $stoptime -and (get-date) -le $stoptimeend)
		{
			try
			{
				#$port.open()
				#$port.Write("@0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,")
				#$port.close
			}
			catch [TimeoutException] 
            {} 
		}
		
		$starttime = get-date "7:30:00 am"
		$starttimeend = $stoptime.AddMinutes(1)
		
		if ((get-date) -ge $starttime -and (get-date) -le $starttimeend)
		{
			try
			{
				#$port.open()
				#$port.Write("@0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,")
				#$port.close()
			}
			catch [TimeoutException] 
            {} 
		}
		
		# Wait 30 seconds and then repeat. You can adjust this time to be more or less but the lower limit should be
		# a second or two greater than any effect that displays on the teensy or you might end up trying to send data 
		# to the teensy while isn't listening. For example, if a rainbow effect takes 20 seconds to complete on the 
		# teensy, you shouldn't make this value any lower than 22 seconds.
		start-sleep -s 30
}