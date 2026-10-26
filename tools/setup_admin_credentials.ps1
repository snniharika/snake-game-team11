$ErrorActionPreference = "Stop"

$iterations = 600000

$securePassword = Read-Host "Enter the Administrator password" -AsSecureString
$passwordPointer = [Runtime.InteropServices.Marshal]::SecureStringToBSTR($securePassword)

try {
    $password = [Runtime.InteropServices.Marshal]::PtrToStringBSTR($passwordPointer)

    if ([string]::IsNullOrEmpty($password)) {
        throw "The password cannot be empty."
    }

    $salt = New-Object byte[] 16

    $rng = [Security.Cryptography.RandomNumberGenerator]::Create()
    try {
        $rng.GetBytes($salt)
    }
    finally {
        $rng.Dispose()
    }

    $deriveBytes = [Security.Cryptography.Rfc2898DeriveBytes]::new(
        $password,
        $salt,
        $iterations,
        [Security.Cryptography.HashAlgorithmName]::SHA256
    )

    try {
        $hash = $deriveBytes.GetBytes(32)
    }
    finally {
        $deriveBytes.Dispose()
    }

    $configDirectory = Join-Path (Get-Location) "config"
    $configPath = Join-Path $configDirectory "admin_credentials.conf"

    New-Item -ItemType Directory -Path $configDirectory -Force | Out-Null

    $saltHex = [System.BitConverter]::ToString($salt).Replace("-", "").ToLowerInvariant()
    $hashHex = [System.BitConverter]::ToString($hash).Replace("-", "").ToLowerInvariant()

    $lines = @(
        "iterations=$iterations"
        "salt=$saltHex"
        "hash=$hashHex"
    )

    Set-Content -Path $configPath -Value $lines -Encoding ascii

    Write-Host "Administrator credential configuration created."
    Write-Host "The password and hash will not be printed."
    Write-Host "Protect config/admin_credentials.conf and do not commit it."
}
finally {
    [Runtime.InteropServices.Marshal]::ZeroFreeBSTR($passwordPointer)
    $password = $null
    $securePassword.Dispose()
}
