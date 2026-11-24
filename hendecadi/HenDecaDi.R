# SPDX-License-Identifier: 0BSD
# HENDECA-RSTAR-001 | PROJECT-AUTHORED SCRIPT ONLY (NO R RUNTIME).
# Program source may be used under its 0BSD license. The interpreter is
# separately obtained by the laboratory; do not vendor, embed or download it.
# Upstream R package license, when independently provisioned: GPL-2 | GPL-3.
# Modern descriptive SPDX alternatives: GPL-2.0-only OR GPL-3.0-only.
# This comment DOES NOT relicense R, grant a future-version option, or
# certify the license of every component of the external R distribution.
# Operator retains its own R source, license, provenance and SHA-256 record.
# AIRGAP: no network or package manager is invoked from this script.
# Offline execution example with separately installed R 4.6.2:
#   Rscript --vanilla hendecadi/HenDecaDi.R input.tsv output.tsv
# NOTE: Logical ATMOSECONDS != measured seconds.
local({
  args <- commandArgs(trailingOnly = TRUE)
  if (length(args) != 2L) stop("RSTAR|HOLD|arguments")
  if (as.character(getRversion()) != "4.6.2")
    stop("RSTAR|HOLD|R_4_6_2_required")
  input <- normalizePath(args[[1L]], mustWork = TRUE)
  output <- args[[2L]]
  if (file.exists(output)) stop("RSTAR|HOLD|output_already_exists")
  records <- readLines(input, warn = FALSE, encoding = "UTF-8")
  if (length(records) != 9L) stop("RSTAR|HOLD|canonical_nine_records")
  if (!all(grepl("^[0-9]+[|][0-9]+[|][0-9]+[|][A-Z0-9_]+[|](SHORT|LONG)[|][0-9]+[|][0-9]+[|][0-9]+$", records)))
    stop("RSTAR|HOLD|invalid_record")
  original <- 0xdbL
  mutated <- bitwXor(original, 0x04L)
  shellA <- bitwXor(0x96L, bitwXor(0x69L, original))
  shellB <- bitwXor(0x96L, bitwXor(0x69L, mutated))
  if (shellA == shellB ||
      bitwOr(0x24L, shellA) != bitwOr(0x24L, shellB))
    stop("RSTAR|HOLD|negative_OR_masking_canary")
  tab <- intToUtf8(9L)
  outputLines <- character(256L)
  outputLines[[1L]] <- paste(c("lap","record","bytes","xor_hex",
                              "or_hex","atmoseconds_logical","elapsed_seconds","result"), collapse=tab)
  for (lap in seq_len(255L)) {
    index <- ((lap - 1L) %% 9L) + 1L
    bytes <- as.integer(charToRaw(enc2utf8(records[[index]])))
    start <- proc.time()[["elapsed"]]
    shell <- bitwXor(0x96L, bitwXor(0x69L, bytes))
    wrap <- bitwOr(0x24L, shell)
    restored <- bitwXor(0x69L, bitwXor(0x96L, shell))
    duration <- proc.time()[["elapsed"]] - start
    if (length(bytes) == 0L || !identical(bytes, restored) ||
        any(shell != bitwXor(bytes, 255L)) ||
        any(wrap != bitwOr(0x24L, bitwXor(bytes, 255L))))
      stop(paste0("RSTAR|HOLD|witness_lap_",lap))
    # n(n+1)/2 is integer-exact at 1..255, NOT measured elapsed time.
    logical <- lap * (lap + 1L) / 2L
    if (logical != floor(logical)) stop("RSTAR|HOLD|noninteger_logical")
    outputLines[[lap + 1L]] <- paste(c(
      as.character(lap),as.character(index),as.character(length(bytes)),
      paste(sprintf("%02x",shell),collapse=""),
      paste(sprintf("%02x",wrap),collapse=""),
      sprintf("%.0f",logical),sprintf("%.17g",duration),"PASS"
    ),collapse=tab)
  }
  writeLines(outputLines,con=output,useBytes=TRUE)
  if (length(readLines(output,warn=FALSE,encoding="UTF-8")) != 256L)
    stop("RSTAR|HOLD|receipt_roundtrip")
  cat("RSTAR|PASS|R=4.6.2|BASE_ONLY=YES|LAPS=255|OR_CANARY=PASS|ATMOSECONDS=LOGICAL_NOT_WALL_TIME|NO_NETWORK=BY_DESIGN|RELEASE=HOLD\n")
})
