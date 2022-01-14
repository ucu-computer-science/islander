output "storage_bucket_id" {
  value = google_storage_bucket.bucket.id
}

output "kms_key_id" {
  value = google_kms_crypto_key.key.id
}

output "gcs_account" {
  value = data.google_storage_project_service_account.gcs_account.email_address
}
