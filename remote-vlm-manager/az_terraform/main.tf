//provider "azurerm" {
//    storage_account_name = var.storage_account_name
//    container_name       = var.container_name
//    key                  = "prod.terraform.tfstate"
//
//    # rather than defining this inline, the Access Key can also be sourced
//    # from an Environment Variable - more information is available below.
//    access_key = var.access_key
//    features {}
//}

terraform {
    required_providers {
        azurerm = {
            source  = "hashicorp/azurerm"
            version = "=2.46.0"
        }
    }
}

# Configure the Microsoft Azure Provider
provider "azurerm" {
    features {}

    use_msi = true

    backend "azurerm" {
        storage_account_name = var.storage_account_name
        container_name       = var.container_name
        key                  = "prod.terraform.tfstate"

        # rather than defining this inline, the Access Key can also be sourced
        # from an Environment Variable - more information is available below.
        access_key = var.access_key
    }
}

module "az_storage" {
    source = "./az_storage"
    container_name = var.container_name
    storage_account_name = var.storage_account_name
}
