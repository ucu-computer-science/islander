resource "azurerm_storage_container" "az_container" {
    name                  = var.container_name
    storage_account_name  = var.storage_account_name
    container_access_type = "private"
}
