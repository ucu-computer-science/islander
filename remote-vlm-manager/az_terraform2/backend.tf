terraform {
  backend "azurerm" {
    storage_account_name = "islander100azstorage"
    container_name       = "tfstate"
    key                  = "prod.terraform.tfstate"

    # rather than defining this inline, the Access Key can also be sourced
    # from an Environment Variable - more information is available below.
    access_key = "FWnmP1qSmeHlAESqEozz/5JoHeaIEKVuUmXrgLwRlUPQLKpTK1WyLolcAr3jaTko0wyoWflmkJ3nFrkf0YZZhQ=="
  }
}
