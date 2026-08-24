// WAVE_init_driver (PoL RVA 0xcc410)
// Buka homolog: soundManager::StartupMilesBackend (RVA 0xb5820).
//
// Reviewed nAvgBytesPerSec arms:
//   bytesPerSample * channels * sampleRate
//   bytesPerSample * sampleRate * channels
//   sampleRate * bytesPerSample * channels
// Artifact: build/source-variant-batch/wave-avg-bytes-product/results.json
// Coverage: complete 3/3 clean-source product; source restored.
//
// Every arm is exact at 330 bytes, 12/12 CFG, and the same complete
// relocation stream. PoL adopts exact-Buka's rate-first product.
// No gameplay or shipped-byte change.
