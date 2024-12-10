#include <clang/AST/ASTConsumer.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Rewrite/Core/Rewriter.h>
#include <clang/Tooling/Tooling.h>

using namespace clang;

class FunctionPointerVisitor : public RecursiveASTVisitor<FunctionPointerVisitor> {
public:
    explicit FunctionPointerVisitor(ASTContext *Context, Rewriter &R)
        : Context(Context), RewriterInstance(R) {
    }

    // Visit CallExpr (function calls)
    bool VisitCallExpr(CallExpr *Call) {
        // Get the callee expression
        Expr *Callee = Call->getCallee();

        // Check if the callee is a function pointer
        if (Callee && Callee->getType()->isFunctionPointerType()) {
            // Get the source location to insert the comment
            SourceLocation EndLoc = Lexer::getLocForEndOfToken(
                Call->getEndLoc(), 0, Context->getSourceManager(), Context->getLangOpts());

            // Insert the comment after the function pointer call
            RewriterInstance.InsertTextAfter(EndLoc, " /*FP*/");
        }

        return true; // Continue traversal
    }

private:
    ASTContext *Context;
    Rewriter &RewriterInstance;
};

class FunctionPointerASTConsumer : public ASTConsumer {
public:
    explicit FunctionPointerASTConsumer(ASTContext *Context, Rewriter &R) : Visitor(Context, R) {
    }

    void HandleTranslationUnit(ASTContext &Context) override {
        Visitor.TraverseDecl(Context.getTranslationUnitDecl());
    }

private:
    FunctionPointerVisitor Visitor;
};

class FunctionPointerFrontendAction : public ASTFrontendAction {
public:
    void EndSourceFileAction() override {
        // Write the rewritten code to stdout
        RewriterInstance.getEditBuffer(RewriterInstance.getSourceMgr().getMainFileID())
            .write(llvm::outs());
    }

    std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, StringRef File) override {
        RewriterInstance.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
        return std::make_unique<FunctionPointerASTConsumer>(&CI.getASTContext(), RewriterInstance);
    }

private:
    Rewriter RewriterInstance;
};

int main(int argc, char **argv) {
    if (argc > 1) {
        clang::tooling::runToolOnCode(std::make_unique<FunctionPointerFrontendAction>(),{},  argv[1]);
    }
    return 0;
}