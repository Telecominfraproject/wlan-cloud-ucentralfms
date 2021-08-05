{{- define "ucentralfms.config" -}}
{{- range $key, $value := .Values.configProperties }}
{{ $key }} = {{ $value }}
{{- end }}
{{- end -}}
