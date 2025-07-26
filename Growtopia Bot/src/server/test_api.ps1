# Growtopia Bot API Test Script (PowerShell)
# Script untuk testing API endpoints menggunakan PowerShell

param(
    [string]$ApiUrl = "http://localhost:8080/api",
    [string]$BotName = "testbot"
)

function Invoke-ApiRequest {
    param(
        [string]$Url,
        [hashtable]$Body,
        [string]$Method = "POST"
    )
    
    try {
        $headers = @{
            "Content-Type" = "application/json"
        }
        
        if ($Method -eq "POST") {
            $jsonBody = $Body | ConvertTo-Json -Depth 10
            $response = Invoke-RestMethod -Uri $Url -Method $Method -Body $jsonBody -Headers $headers
        } else {
            $response = Invoke-RestMethod -Uri $Url -Method $Method -Headers $headers
        }
        
        return $response
    }
    catch {
        Write-Host "Error: $($_.Exception.Message)" -ForegroundColor Red
        return $null
    }
}

function Test-ApiEndpoint {
    param(
        [string]$TestName,
        [hashtable]$RequestBody,
        [string]$Method = "POST"
    )
    
    Write-Host "\n[$TestName]" -ForegroundColor Cyan
    Write-Host "Request: $($RequestBody | ConvertTo-Json -Compress)" -ForegroundColor Gray
    
    $result = Invoke-ApiRequest -Url $ApiUrl -Body $RequestBody -Method $Method
    
    if ($result) {
        Write-Host "Response: $($result | ConvertTo-Json -Depth 10)" -ForegroundColor Green
    } else {
        Write-Host "No response or error occurred" -ForegroundColor Red
    }
    
    Start-Sleep -Milliseconds 500
}

Write-Host "========================================" -ForegroundColor Yellow
Write-Host "Growtopia Bot API Test Script (PowerShell)" -ForegroundColor Yellow
Write-Host "========================================" -ForegroundColor Yellow
Write-Host "API URL: $ApiUrl"
Write-Host "Bot Name: $BotName"
Write-Host ""

# Test 1: Health Check
Write-Host "\n[Health Check]" -ForegroundColor Cyan
try {
    $healthResult = Invoke-RestMethod -Uri "$ApiUrl/ok" -Method GET
    Write-Host "Health Check: $($healthResult | ConvertTo-Json)" -ForegroundColor Green
} catch {
    Write-Host "Health Check Failed: $($_.Exception.Message)" -ForegroundColor Red
}

# Test 2: List all clients
Test-ApiEndpoint -TestName "List Clients" -RequestBody @{
    option = "list_clients"
}

# Test 3: Get client info
Test-ApiEndpoint -TestName "Client Info" -RequestBody @{
    option = "client"
    tank_id_name = $BotName
}

# Test 4: Get inventory
Test-ApiEndpoint -TestName "Inventory" -RequestBody @{
    option = "inventory"
    tank_id_name = $BotName
}

# Test 5: Get player info
Test-ApiEndpoint -TestName "Player Info" -RequestBody @{
    option = "player_info"
    tank_id_name = $BotName
}

# Test 6: Get world info
Test-ApiEndpoint -TestName "World Info" -RequestBody @{
    option = "world_info"
    tank_id_name = $BotName
}

# Test 7: Get status
Test-ApiEndpoint -TestName "Bot Status" -RequestBody @{
    option = "status"
    tank_id_name = $BotName
}

# Test 8: Get ping
Test-ApiEndpoint -TestName "Bot Ping" -RequestBody @{
    option = "ping"
    tank_id_name = $BotName
}

# Test 9: Move bot
Test-ApiEndpoint -TestName "Move Bot" -RequestBody @{
    option = "move"
    tank_id_name = $BotName
    x = 100
    y = 200
}

# Test 10: Teleport bot
Test-ApiEndpoint -TestName "Teleport Bot" -RequestBody @{
    option = "teleport"
    tank_id_name = $BotName
    x = 150
    y = 250
}

# Test 11: Collect items
Test-ApiEndpoint -TestName "Collect Items" -RequestBody @{
    option = "collect"
    tank_id_name = $BotName
    range = 3
    force = $false
}

# Test 12: Punch block
Test-ApiEndpoint -TestName "Punch Block" -RequestBody @{
    option = "punch"
    tank_id_name = $BotName
    x = 100
    y = 200
}

# Test 13: Place block (dirt)
Test-ApiEndpoint -TestName "Place Block" -RequestBody @{
    option = "place"
    tank_id_name = $BotName
    item_id = 2
    x = 100
    y = 200
}

# Test 14: Send message
Test-ApiEndpoint -TestName "Send Message" -RequestBody @{
    option = "send"
    tank_id_name = $BotName
    message = "Hello from API test!"
}

# Test 15: Smoke
Test-ApiEndpoint -TestName "Smoke" -RequestBody @{
    option = "smoke"
    tank_id_name = $BotName
}

Write-Host "\n========================================" -ForegroundColor Yellow
Write-Host "API Testing completed!" -ForegroundColor Yellow
Write-Host "========================================" -ForegroundColor Yellow
Write-Host ""
Write-Host "Note: Make sure the Growtopia Bot application is running" -ForegroundColor Cyan
Write-Host "and you have a bot named '$BotName' added to test properly." -ForegroundColor Cyan
Write-Host ""
Write-Host "To add a bot, run:" -ForegroundColor Yellow
Write-Host "Invoke-RestMethod -Uri '$ApiUrl' -Method POST -Body '{\"option\": \"add\", \"tank_id_name\": \"$BotName\", \"tank_id_pass\": \"your_password\", \"server\": \"growtopia1.com\", \"port\": 17091}' -ContentType 'application/json'" -ForegroundColor Gray