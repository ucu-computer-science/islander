terraform {
  backend "azurerm" {
    storage_account_name = "<YOUR-ACCOUNT-NAME>"
    container_name       = "tfstate"
    key                  = "prod.terraform.tfstate"

    # rather than defining this inline, the Access Key can also be sourced
    # from an Environment Variable - more information is available below.
    access_key = "<YOUR-ACCESS-KEY>"
  }
}
