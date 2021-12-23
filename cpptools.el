;; Eval Buffer with `M-x eval-buffer' to register the newly created template.

(dap-register-debug-template
  "MDViewer cpptools::Run Configuration"
  (list :type "cppdbg"
        :request "launch"
        :name "cpptools::Run Configuration"
        :MIMode "gdb"
        :program "${workspaceFolder}/main"
        :cwd "${workspaceFolder}"))
