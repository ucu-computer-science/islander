provider "aws" {
    access_key = "${var.aws_access_key}"
    secret_key = "${var.aws_secret_key}"
    region = "${var.region}"
}

module "s3" {
    # source = "../terraform_modules/"
    source = "./s3"
    # bucket name should be unique
    bucket_name = "os-project-test2"

    # bucket = "os-project-test"
    # acl    = "private"       

    # versioning = {
    #     enabled = true
    # }
}
