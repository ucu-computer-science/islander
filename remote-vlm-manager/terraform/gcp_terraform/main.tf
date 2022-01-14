# Configure the Google Cloud provider
provider "google" {
  project = "<YOUR_PROJECT_NAME>"
  region  = "europe-central2"
}

data "google_project" "project" {}

data "google_storage_project_service_account" "gcs_account" {
}

resource "random_id" "rand" {
  byte_length = 4
  keepers = {
    key_name = var.key_name
    bucket_name = var.bucket_name
  }
}

resource "google_kms_key_ring" "keyring" {
  name = lower("${var.keyring_name}-${random_id.rand.hex}")
  location = var.region
}

resource "google_kms_crypto_key" "key" {
  name = var.key_name
  key_ring = google_kms_key_ring.keyring.id
  rotation_period = var.rotation_period

  version_template {
    algorithm = var.algorithm
  }

  lifecycle {
    prevent_destroy = false
  }
}

resource "google_kms_crypto_key_iam_binding" "crypto_key" {

  crypto_key_id = google_kms_crypto_key.key.id
  role          = "roles/cloudkms.cryptoKeyEncrypterDecrypter"

  members       = [
    "serviceAccount:${data.google_storage_project_service_account.gcs_account.email_address}",
  ]
}

# Create a Google Cloud Storage Bucket
resource "google_storage_bucket" "bucket" {
  name          = lower("${var.bucket_name}-${random_id.rand.hex}")
  location      = var.region

  uniform_bucket_level_access = true
  storage_class = var.storage_class

  versioning {
    enabled     = true
  }

  encryption {
    default_kms_key_name = google_kms_crypto_key.key.id
  }

  lifecycle_rule {
    action {
      type = "Delete"
    }
    condition {
      age = 1
    }
  }

  force_destroy = true

  depends_on = [
    google_kms_crypto_key_iam_binding.crypto_key
  ]
}
