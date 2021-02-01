Push-Location
$scriptdir = Split-Path $MyInvocation.MyCommand.Path
cd $scriptdir


$nugetFiles = @(gci *.nupkg)


ForEach ($nugetFile in $nugetFiles) {
    Write-Host "nuget file = " $nugetFile

	#Open zip
	Add-Type -assembly System.IO.Compression.FileSystem
	$zip =  [System.IO.Compression.ZipFile]::Open($nugetFile,"Update")
	If(!$zip){
		Write-Host "error: could not open nuget file"
		exit 1
	}
	
	#find targets file
	Try{
		$targetsFilenames = $zip.Entries -like "build/native/*.targets" #.Where({$_.name -match "build/native/*.targets"})

		ForEach ($targetsFilename in $targetsFilenames){
			Write-Host ".targets file = " $targetsFilename
		
			# Read the contents of the file
			$targetsFileEntry = $zip.GetEntry($targetsFilename)   #[System.IO.StreamReader]($targetsFilename).Open()
			If(!$targetsFileEntry){
				Write-Host "error: could not open targets file"
				exit 1
			}
			$targetsFile = [System.IO.StreamReader]($targetsFileEntry.Open())
			Try{
				$targetsText =  $targetsFile.ReadToEnd()
			}Finally{
				$targetsFile.Close()
				$targetsFile.Dispose()
			}
			
			#$targetsText
			$xml = [xml]$targetsText
			
			$ns = New-Object System.Xml.XmlNamespaceManager($xml.NameTable)
			$ns.AddNamespace("ns", $xml.DocumentElement.NamespaceURI)
			$nodes = $xml.SelectNodes("//ns:Copy", $ns)

			#$nodes
			ForEach($node in $nodes){
				$path = $node.SourceFiles
				$path = $path -replace  ".*build[\/\\]native[\/\\]bin[\/\\]x64[\/\\]v\d*[\/\\]Debug[\/\\]MultiThreadedDebugDLL(.*)[\/\\][^\/\\]*$",'$1'
				$path = $path -replace  ".*build[\/\\]native[\/\\]bin[\/\\]Win32[\/\\]v\d*[\/\\]Debug[\/\\]MultiThreadedDebugDLL(.*)[\/\\][^\/\\]*$",'$1'
				$path = $path -replace  ".*build[\/\\]native[\/\\]bin[\/\\]x64[\/\\]v\d*[\/\\]Debug[\/\\]MultiThreadedDebug(.*)[\/\\][^\/\\]*$",'$1'
				$path = $path -replace  ".*build[\/\\]native[\/\\]bin[\/\\]Win32[\/\\]v\d*[\/\\]Debug[\/\\]MultiThreadedDebug(.*)[\/\\][^\/\\]*$",'$1'
				$path = $path -replace  ".*build[\/\\]native[\/\\]bin[\/\\]x64[\/\\]v\d*[\/\\]Release[\/\\]MultiThreadedDLL(.*)[\/\\][^\/\\]*$",'$1'
				$path = $path -replace  ".*build[\/\\]native[\/\\]bin[\/\\]Win32[\/\\]v\d*[\/\\]Release[\/\\]MultiThreadedDLL(.*)[\/\\][^\/\\]*$",'$1'
				$path = $path -replace  ".*build[\/\\]native[\/\\]bin[\/\\]x64[\/\\]v\d*[\/\\]Release[\/\\]MultiThreaded(.*)[\/\\][^\/\\]*$",'$1'
				$path = $path -replace  ".*build[\/\\]native[\/\\]bin[\/\\]Win32[\/\\]v\d*[\/\\]Release[\/\\]MultiThreaded(.*)[\/\\][^\/\\]*$",'$1'
				
				#$path
				#$node.DestinationFolder
				$base = '$(LocalDebuggerWorkingDirectory)'
				$node.SetAttribute("DestinationFolder", "$($base)$($path)")
				#$node.DestinationFolder
			}
			
			$targetsFile = [System.IO.StreamWriter]($targetsFileEntry.Open())
			Try{
				# If needed, zero out the file -- in case the new file is shorter than the old one
				$targetsFile.BaseStream.SetLength(0)

				#$xml.OuterXml
				
				Write-Host "updating zip file..."
				$targetsFile.Write($xml.OuterXml -join "`r`n")
			}Finally{
				$targetsFile.Close()
				$targetsFile.Dispose()
			}
		}
	}Finally{
		Write-Host "closing zip file..."
		$zip.Dispose()
	}
}

Pop-Location
